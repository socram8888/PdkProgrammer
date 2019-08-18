/*
 * pdkspi.h
 *
 * Created: 18/08/2019 14:21:34
 *  Author: Marcos
 */ 


#ifndef PDKSPI_H_
#define PDKSPI_H_

#include <stdint.h>

void padauk_init();

uint16_t padauk_start_read();
uint16_t padauk_flash_read(uint16_t addr);

uint16_t padauk_start_write();
void padauk_flash_write(uint16_t addr, const uint16_t * data);

uint16_t padauk_erase();

void padauk_finish();

#endif /* PDKSPI_H_ */
