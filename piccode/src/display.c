#include "main.h"
#include "display.h"
#include "lcd.c"
#include "pimetaldec.h"

void dsp_init() {
    lcd_init();
}

void dsp_clear() {
	lcd_putc( '\f' );
}

void dsp_hello() {
	lcd_putc( '\f' );
	printf( lcd_putc, "PI METAL DETEC.\n" );
	printf( lcd_putc, "A. & G. Corp." );
}


void dsp_setup_coil_pulse_ref( int16 reference_5v ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "SET: coil pulse\n" );
	printf( lcd_putc, "5V ref.: %Lu", reference_5v );
}


void dsp_setup_coil_pulse( int16 coil_volts ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "SET: coil pulse\n" );
	printf( lcd_putc, "%Luus  -->  %LuV", pi.pulse_time, coil_volts );
}


void dsp_setup_sample_delay( int8 strength ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "SET: samp. delay\n" );
	printf( lcd_putc, "%uus  -->  %u", pi.start_sample_delay, strength );
}


void dsp_sample( int8 strength ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "Strenght: %u", strength );
}


void dsp_data( int16 value ) {
	lcd_putc( '\f' );
	printf( lcd_putc, "P: %Lu", pi.pulse_time );
    lcd_gotoxy( 10, 1 );
    printf( lcd_putc, "S: %u\n", pi.start_sample_delay );
	printf( lcd_putc, "I: %u", pi.sample_time );
    lcd_gotoxy( 10, 2 );
    printf( lcd_putc, "V: %Lu\n", value );
}