/*
 * PWM Buzzer.c
 *
 * Created: 10-Jun-26 12:17:49 PM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BUZZER_PIN  PC5
#define BUZZER_DDR  DDRC
#define BUZZER_PORT PORTC

#define BUTTON_PIN  PD2
#define BUTTON_DDR  DDRD
#define BUTTON_PORT PORTD

ISR(TIMER2_COMP_vect)
{
	PORTC ^= (1 << BUZZER_PIN);
}

void buzzer_tone(uint8_t ocr_value)
{
	OCR2  = ocr_value;
	TCNT2 = 0;
	TCCR2 = (1 << WGM21) | (1 << CS22);
	TIMSK |= (1 << OCIE2);
}

void buzzer_off(void)
{
	TCCR2 = 0;
	TIMSK &= ~(1 << OCIE2);
	BUZZER_PORT &= ~(1 << BUZZER_PIN);
}

int main(void)
{
	BUZZER_DDR |= (1 << BUZZER_PIN);

	BUTTON_DDR  &= ~(1 << BUTTON_PIN);
	BUTTON_PORT |=  (1 << BUTTON_PIN);

	sei();

	uint8_t counter      = 0;
	uint8_t prev_state   = 1;

	while (1)
	{
		uint8_t curr_state = (PIND & (1 << BUTTON_PIN)) ? 1 : 0;

		if (prev_state == 1 && curr_state == 0)
		{
			_delay_ms(20);
			if (!(PIND & (1 << BUTTON_PIN)))
			{
				counter++;
				if (counter > 3) counter = 0;

				if (counter == 0) buzzer_off();
				else if (counter == 1) buzzer_tone(124);
				else if (counter == 2) buzzer_tone(61);
				else if (counter == 3) buzzer_tone(30);
			}
		}

		prev_state = curr_state;
	}
}