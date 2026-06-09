/*
 * GPIO Button Input.c
 *
 * Created: 09-Jun-26 11:50:05 AM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define push_button PD2
#define green_led PC1

int main(void)
{
    DDRD &= ~(1 << push_button);
	PORTD |= (1 << push_button);
	DDRC |= (1 << green_led);
	
	
    while (1) 
    {
		if (!(PIND & (1 << push_button)))
		{
			_delay_ms(20);
			if (!(PIND & (1 << push_button)))
				PORTC |= (1 << green_led);
		}
		else
			PORTC &= ~(1 << green_led);
    }
}

