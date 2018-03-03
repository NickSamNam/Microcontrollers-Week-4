/*
 * B2.c
 *
 * Created: 3-3-2018 17:31:12
 * Author : snick
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>

#include "HD44780U.h"

/* Reference voltage (mV) */
#define Vref 5120

typedef unsigned long int u_int32;
typedef unsigned int u_int16;
typedef unsigned char u_char;

/* Measured voltage (mV) */
u_int16 v = 0x0000;

/* Dirty flag measured voltage */
u_char vf = 0;

void wait(int ms)
{
	int i;
	for (i = 0; i < ms; i++)
	{
		_delay_ms(1);
	}
}

ISR(ADC_vect)
{
	u_int32 result = ADCH << 2; // Take most important 8 bits
	result *= Vref;
	result /= 1023;
	v = result;
	vf = 1; // Set dirty
}

int main(void)
{
	// Init IO
	DDRA = 0xFF;	// PA0-PA7 Write
	DDRB = 0xFF;	// PB0-PB7 Write
	DDRF &= ~0x08;	// PF3(ADC3) Read
	
	// Init ADC
	ADMUX = 0x23;	// ADC3, Vref off, left adjust result
	ADCSRA = 0x8A;	// Enable ADC, interrupt enabled, prescaler 4
	
	sei();	// Enable global interrupt
	
	LCD_init();
	LCD_display_text("Voltage: 0000mV");
	
	while (1)
	{
		if (vf)
		{
			LCD_set_cursor(9);
			char vs[5] = "";
			sprintf(vs, "%04u", v);
			LCD_display_text(vs);
			vf = 0; // Clear dirty
		}
		else
		{
			ADCSRA |= 0x40; // Start conversion
			wait(100);
		}
	}
}
