/*
 * uart.h
 *
 * Created: 09/02/2019 20:39:26
 *  Author: Marcos
 */ 

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void USART_Init(uint16_t ubrr);
void USART_Transmit(uint8_t data);
uint8_t USART_Receive(void);

#endif /* UART_H_ */
