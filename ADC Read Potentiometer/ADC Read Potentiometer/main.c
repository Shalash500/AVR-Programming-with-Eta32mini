/*
 * ADC Read Potentiometer.c
 *
 * Created: 10-Jun-26 1:43:48 PM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define UBRR_VAL 103

void uart_init(void)
{
	UBRRH = (UBRR_VAL >> 8);
	UBRRL = (UBRR_VAL & 0xFF);
	UCSRB = (1 << TXEN);
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void uart_send_byte(uint8_t data)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = data;
}

void uart_send_string(const char *str)
{
	while (*str)
	{
		uart_send_byte(*str);
		str++;
	}
}

void uart_send_number(uint16_t num)
{
	char buf[6];
	buf[5] = '\0';
	buf[4] = '0' + (num % 10); num /= 10;
	buf[3] = '0' + (num % 10); num /= 10;
	buf[2] = '0' + (num % 10); num /= 10;
	buf[1] = '0' + (num % 10); num /= 10;
	buf[0] = '0' + (num % 10);
	uart_send_string(buf);
}

void adc_init(void)
{
	ADMUX  = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

uint16_t adc_read(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int main(void)
{
	uart_init();
	adc_init();

	while (1)
	{
		uint16_t value = adc_read();
		uart_send_string("ADC: ");
		uart_send_number(value);
		uart_send_string("\r\n");
		_delay_ms(500);
	}
}