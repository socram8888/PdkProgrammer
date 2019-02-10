/*
 * uart.c
 *
 * Created: 09/02/2019 20:24:17
 *  Author: Marcos
 */ 

#include "uart.h"
#include <avr/io.h>

#ifndef _BV
#define _BV(x) (1 << x)
#endif

void USART_Init(uint16_t ubrr) {
	/* Set baud rate */
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (3 << UCSZ00);
}

void USART_Transmit(uint8_t data) {
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & _BV(UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

uint8_t USART_Receive(void) {
	/* Wait for data to be received */
	while (!(UCSR0A & _BV(RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}
