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
#include "pcproto.h"
#include "pdkspi.h"

uint8_t currentCommand = 0;
uint8_t currentMode = MODE_OFF;

struct request request;
uint8_t requestBytesPos;
uint8_t requestBytesLen;

struct reply reply;

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

#define SMPS_EN_DDR DDRA
#define SMPS_EN_PORT PORTA
#define SMPS_EN_BIT 6

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t* rq = (void*) data;

	requestBytesPos = 0;
	requestBytesLen = (uint8_t) rq->wLength.word;

	switch (rq->bRequest) {
		case CMD_SET_MODE:
			// No data expected
			if (requestBytesLen == 0) {
				reply.mode.devId = 0;
				if (currentMode != MODE_OFF) {
					padauk_finish();
					currentMode = MODE_OFF;
				}

				switch (rq->wValue.bytes[0]) {
					case MODE_READ:
						reply.mode.devId = padauk_start_read();
						currentMode = MODE_READ;
						break;
					case MODE_WRITE:
						reply.mode.devId = padauk_start_write();
						currentMode = MODE_WRITE;
						break;
				}

				reply.mode.currentMode = currentMode;
				usbMsgPtr = (uint8_t *) &reply;
				return sizeof(reply.mode);
			}
			break;

		case CMD_READ:
			if (requestBytesLen == sizeof(struct request_read)) {
				currentCommand = CMD_READ;
				return USB_NO_MSG;
			}
			break;

		case CMD_WRITE:
			if (
				requestBytesLen >= 4 && // At least one word to write
				requestBytesLen % 2 == 1 && // Only pairs of bytes
				requestBytesLen < sizeof(struct request_write) // Do not exceed buffer size
			) {
				currentCommand = CMD_WRITE;
				return USB_NO_MSG;
			}
			break;
	}

	return 0;
}

USB_PUBLIC uchar usbFunctionWrite(uchar * data, usbMsgLen_t len) {
	uint8_t * requestBytes = (uint8_t *) &request;
	for (uint8_t i = 0; i < len; i++) {
		requestBytes[requestBytesPos + i] = data[i];
	}
	requestBytesPos += len;

	if (requestBytesPos < requestBytesLen) {
		return 0;
	}

	_delay_ms(1000);
	return 1;
}
