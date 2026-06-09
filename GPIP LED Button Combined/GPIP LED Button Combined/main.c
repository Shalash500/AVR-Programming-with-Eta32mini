/*
 * GPIP LED Button Combined.c
 *
 * Created: 09-Jun-26 12:56:31 PM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define push_button1 PD2
#define push_button2 PD3

#define red_led PC0
#define green_led PC1
#define blue_led PC2

uint8_t state = 0;

void all_leds_off(void)
{
	PORTC &= ~((1 << red_led) | (1 << green_led) | (1 << blue_led));
}

int main(void)
{
	DDRC |= (1 << red_led) | (1 << green_led) | (1 << blue_led);
    DDRD &= ~((1 << push_button1) | (1 << push_button2));
	PORTD |= ((1 << push_button1) | (1 << push_button2));
	uint8_t current_state = 0;
	uint8_t previous_state = 1;
	uint8_t sw2_previous = 1;
    while (1) 
    {
		if (PIND & (1 << push_button1))
			current_state = 1;
		else
		current_state = 0;
			if (previous_state == 1 && current_state == 0)  // falling edge detected
		{
			_delay_ms(20);

			if (!(PIND & (1 << push_button1)))
			{
				state++;
			}
		}
		
		if (state == 0)
		{
			all_leds_off();
			PORTC |= (1 << red_led);
		}
		else if (state == 1)
		{
			all_leds_off();
			PORTC |= (1 << green_led);
		}
		else if (state == 2)
		{
			all_leds_off();
			PORTC |= (1 << blue_led);
		}
		else if (state > 2)
			state = 0;
		
		uint8_t sw2_current = (PIND & (1 << push_button2)) ? 1 : 0;

		if (sw2_previous == 1 && sw2_current == 0)
		{
			_delay_ms(20);
			if (!(PIND & (1 << push_button2)))
			{
				all_leds_off();
				state = 0;
			}
		}
		sw2_previous = sw2_current;

		previous_state = current_state;
    }
}

