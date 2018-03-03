/*
 * B3.c
 *
 * Created: 3-3-2018 18:29:36
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

/* Temperature in 1/10 C */
int temp = 0;

/* temp dirty flag */
u_char tf = 0;

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
	PORTA = result;
	result *= Vref;
	result /= 1023;
	temp = result;
	tf = 1;
}

int main(void)
{
	// Init IO
	DDRA = 0xFF;	// PA0-PA7 Write
	DDRF &= ~0x01;	// PF0(ADC0) Read
	
	// Init ADC
	ADMUX = 0x20;	// ADC0, Vref off, left adjust result
	ADCSRA = 0x8A;	// Enable ADC, interrupt enabled, prescaler 4
	
	sei();	// Enable global interrupt
	
	LCD_init();
	LCD_display_text("Temp:");
	
	ADCSRA |= 0x40; // Start conversion
	
	while (1)
	{
		if (tf)
		{
			LCD_set_cursor(6);
			char ts[8] = "";
			sprintf(ts, "%d.%dC  ", temp / 10, temp % 10);
			LCD_display_text(ts);
			tf = 0; // Clear dirty
			wait(100);
			ADCSRA |= 0x40; // Start conversion
		}
		asm("NOP"); // needed for some reason
	}
}
