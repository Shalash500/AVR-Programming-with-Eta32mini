/*
 * GPIO Traffic Light.c
 *
 * Created: 09-Jun-26 11:03:11 AM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define RED_LED PC0
#define GREEN_LED PC1
#define BLUE_LED PC2

void all_leds_off(void)
{
	PORTC &= ~((1 << RED_LED) | (1 << GREEN_LED) | (1 << BLUE_LED));
}

int main(void)
{
    DDRC |= ((1 << RED_LED) | (1 << GREEN_LED) | (1 << BLUE_LED));
    while (1) 
    {
		PORTC |= (1 << RED_LED);
		_delay_ms(1000);
		
		all_leds_off();
		_delay_ms(200);
		
		PORTC |= (1 << GREEN_LED);
		_delay_ms(1000);
		
		all_leds_off();
		_delay_ms(200);
		
		PORTC |= (1 << BLUE_LED);
		_delay_ms(1000);
		
		all_leds_off();
		_delay_ms(200);
    }
}
