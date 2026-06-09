/*
 * GPIO LED Blink.c
 *
 * Created: 09-Jun-26 10:46:54 AM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC |= (1 << PC1);

	while (1)
	{
		PORTC |= (1 << PC1);
		_delay_ms(500);
		PORTC &= ~(1 << PC1);
		_delay_ms(500);
	}

	return 0;
}
