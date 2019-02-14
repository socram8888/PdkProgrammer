/*
 * debug.c
 *
 * Created: 12/02/2019 17:19:33
 *  Author: Marcos
 */ 

#include "uart.h"

void log_init() {
	// 57600 baud
	USART_Init(16);
}

void log_char(char c) {
	USART_Transmit(c);
}

void log_nibble_hex(uint8_t val) {
	val &= 0xF;
	if (val >= 0xA) {
		log_char(val + 'A' - 0xA);
		} else {
		log_char(val + '0');
	}
}

void log_hex(uint8_t val) {
	log_nibble_hex(val >> 4);
	log_nibble_hex(val);
}

void log_hex16(uint16_t val) {
	log_hex(val >> 8);
	log_hex(val);
}
