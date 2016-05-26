#include "main.h"
#include "input.h"

#define CHANGE_CONFIDENT_THRESHOLD 2

//At 16Mhz counter increments every 64us:
//	- For 250 counts, overflows every 16ms
#define TIMER_INIT_TIME (256-250)
#define TIMER_PERIOD_MS 16

struct InSwitch in_switches[SWITCHES_SIZE];

void in_init() {
    output_float( PI_BUTTON_MODE_PIN );
    output_float( PI_BUTTON_AUTO_PIN );
    output_float( PI_BUTTON_INC_PIN );
    output_float( PI_BUTTON_DEC_PIN );
    
    RBPU = 0;                              //Enable pull-up resistors on PORT B
    WPUB = PI_BUTTON_PULLUP_RESISTORS;     //Enable pull-ups for buttons
    
    in_switches[SWITCH_MODE].pin = PI_BUTTON_MODE_PIN;
    in_switches[SWITCH_AUTOSET].pin = PI_BUTTON_AUTO_PIN;
    in_switches[SWITCH_INCREMENT].pin = PI_BUTTON_INC_PIN;
    in_switches[SWITCH_DECREMENT].pin = PI_BUTTON_DEC_PIN;
	
	for( int8 i=0; i<SWITCHES_SIZE; ++i ) {
		in_switches[i].changed = FALSE;
	}
    
    
    setup_timer_0( T0_INTERNAL | T0_DIV_256 | T0_8_BIT );
    set_timer0(TIMER_INIT_TIME);
    enable_interrupts(INT_TIMER0);
}


#int_timer0
void isr_timer0() {
	set_timer0(TIMER_INIT_TIME);
	
    int8 i;
    for( i=0; i<SWITCHES_SIZE; ++i ) {
        struct InSwitch* sw = &in_switches[i];
        
        int1 current_state = !input( sw->pin );
		
		if ( current_state == sw->state ) {
			sw->change_confident_count = 0;
			if ( sw->state_time < (0xFFFF - TIMER_PERIOD_MS) )
				sw->state_time += TIMER_PERIOD_MS;
		}
		else {
			if ( ++sw->change_confident_count == CHANGE_CONFIDENT_THRESHOLD ) {
				sw->changed = TRUE;
				sw->state = current_state;
				sw->state_time = 0;
			}
		}
    }
}



// Wait until button releases or it has been pressed for more than 
// max_push_time ms. Returns true if max_push_time has been 
// reached.
int1 in_wait_for_release_timeout( int8 sw, int16 max_push_time ) {
    while( in_switches[sw].state ) {
		if ( in_switches[sw].state_time > max_push_time )
			return TRUE;
    }
    return FALSE;
}


void in_wait_for_release( int8 sw ) {
	while( in_switches[sw].state );
}


int1 in_button_pressed( int8 sw ) {
	int1 ret = in_switches[sw].state && in_switches[sw].changed;
	in_switches[sw].changed = FALSE;
	return ret;
}


///////////////////////////////////////////////////////////////
// Increment switches
///////////////////////////////////////////////////////////////

struct {
	int16 min;
	int16 max;
	int16 rate;
} in_increment_state;


void in_init_increment( int16 min, int16 max, int16 rate ) {
	in_switches[SWITCH_INCREMENT].state_time = 0;
	in_switches[SWITCH_DECREMENT].state_time = 0;
	
	in_increment_state.min = min;
	in_increment_state.max = max;
	
	if ( rate == INCREMENT_AUTO_RATE ) {
		rate = 15000 / (max-min);		//All range in 15s
	}
	in_increment_state.rate = rate;
}


int8 pulse_time_increment( int8 sw ) {
	int8 ret = in_switches[sw].state_time / in_increment_state.rate;
	in_switches[sw].state_time -= (ret*in_increment_state.rate);
	return ret;
}


signed int16 in_increment( int16 current ) {
	signed int16 increment = 0;
	
	if ( in_switches[SWITCH_INCREMENT].state ) {
		int16 raw_inc = pulse_time_increment( SWITCH_INCREMENT );
		increment = in_increment_state.max - current;
		increment = (increment>raw_inc) ? raw_inc : increment;
	}
	
	if ( in_switches[SWITCH_DECREMENT].state ) {
		int16 raw_dec = pulse_time_increment( SWITCH_DECREMENT );
		increment = current - in_increment_state.min;
		increment = (increment>raw_dec) ? -raw_dec : -increment;
	}
	
	return increment;
}