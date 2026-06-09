/*
 * Timers CTC Mode Blink.c
 *
 * Created: 09-Jun-26 2:49:29 PM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t counter = 0;
uint16_t OCR1A_value = 7811;

#define push_button PD2
#define green_led PC1

ISR(TIMER1_COMPA_vect)
{
	PORTC ^= (1 << green_led);
}

void timer1_init(void)
{
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	OCR1A  = 7811;
	TIMSK |= (1 << OCIE1A);
}

int main(void)
{
	DDRD &= ~(1 << push_button);
	PORTD |= (1 << push_button);
	DDRC |= (1 << green_led);
	
	uint8_t current_state = 0;
	uint8_t previous_state = 1;
	
	timer1_init();
	sei();

	while (1)
	{
		if (PIND & (1 << push_button))
		current_state = 1;
		else
		current_state = 0;
		if (previous_state == 1 && current_state == 0)  // falling edge detected
		{
			_delay_ms(20);

			if (!(PIND & (1 << push_button)))
			{
				counter++;
				if (counter > 2)
					counter = 0;
			}
			if (counter == 0)
			{
				OCR1A_value = 7811;
			}
			else if (counter == 1)
			OCR1A_value = 3905;
			else if (counter == 2)
			OCR1A_value = 1562;
			OCR1A = OCR1A_value;
			TCNT1 = 0;
		}
		previous_state = current_state;
	}
}