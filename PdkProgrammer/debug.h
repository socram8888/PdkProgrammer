/*
 * debug.h
 *
 * Created: 12/02/2019 17:19:56
 *  Author: Marcos
 */ 


#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>

void log_init();
void log_char(char c);
void log_hex(uint8_t val);
void log_hex16(uint16_t val);

#endif /* DEBUG_H_ */
