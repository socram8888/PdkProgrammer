/*
 * PdkProgrammer.c
 *
 * Created: 09/02/2019 15:30:34
 * Author : Marcos
 */ 

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "debug.h"
#include "smps.h"

int main(void) {
	// SMPS enable pin as output
	smps_init();

	// Enable serial logging
	log_init();

	sei();

    /* Replace with your application code */
    while (1) 
    {
		log_hex(1);
		log_char('\n');
		smps_on(smps_adc_target(5.8));
		//_delay_ms(1000);
		log_hex(2);
		log_char('\n');
		smps_switch(smps_adc_target(6.2));
		//_delay_ms(1000);
		log_hex(3);
		log_char('\n');
		smps_off();
		//_delay_ms(1000);
		log_char('\n');
    }
}

