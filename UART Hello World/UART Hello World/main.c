/*
 * UART Hello World.c
 *
 * Created: 10-Jun-26 1:09:30 PM
 * Author : Ahmed
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BAUD     9600
#define UBRR_VAL 103

void uart_init(void)
{
	// Set baud rate
	UBRRH = (UBRR_VAL >> 8);   // high byte
	UBRRL = (UBRR_VAL & 0xFF); // low byte

	// Enable transmitter only for now
	UCSRB = (1 << TXEN);

	// Frame format: 8 data bits, 1 stop bit
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void uart_send_byte(uint8_t data)
{
	// Wait until transmit buffer is empty
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

int main(void)
{
	uart_init();

	while (1)
	{
		uart_send_string("Hello from ATmega32!\r\n");
		_delay_ms(1000);
	}
}
