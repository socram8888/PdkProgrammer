/*
 * PdkProgrammer.c
 *
 * Created: 09/02/2019 15:30:34
 * Author : Marcos
 */ 

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "vusb/usbconfig.h"
#include "vusb/usbdrv.h"

#include "smps.h"

int main(void) {
	// SMPS enable pin as output
	//smps_init();

	// Initialize USB
	usbInit();

	// Force USB re-enumeration
	usbDeviceDisconnect();
	_delay_ms(250);
	usbDeviceConnect();

	sei();

    /* Replace with your application code */
    while (1) 
    {
		/*
		smps_on(smps_adc_target(5.8));
		_delay_ms(1000);
		smps_switch(smps_adc_target(6.2));
		_delay_ms(1000);
		smps_off();
		_delay_ms(1000);
		*/
		usbPoll();
    }
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t* rq = (void*) data;
	return 0;
}