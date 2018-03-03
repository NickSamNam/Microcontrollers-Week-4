/*
* B1.c
*
* Created: 3-3-2018 12:47:11
* Author : snick
*/

#include <avr/io.h>
#include <avr/interrupt.h>

/* Reference voltage (mV) */
#define Vref 5120


typedef unsigned long int u_int32;
typedef unsigned int u_int16;

/* Measured voltage (mV) */
u_int16 v = 0x0000;

ISR(ADC_vect)
{
	u_int32 result = ADC;
	result *= Vref;
	result /= 1023;
	v = result;
}

int main(void)
{
	// Init IO
	DDRA = 0xFF;	// PA0-PA7 Write
	DDRB = 0xFF;	// PB0-PB7 Write
	DDRF &= ~0x02;	// PF1(ADC1) Read
	
	// Init ADC
	ADMUX = 0x01;	// ADC1, Vref off, right adjust result
	ADCSRA = 0xEE;	// Enable ADC, start conversion, free running, interrupt enabled, prescaler 64
	
	sei();	// Enable global interrupt
	
	while (1)
	{
		PORTA = PORTB = (0xFF << (8 - v * 8 / Vref));	// Show voltage
	}
}
