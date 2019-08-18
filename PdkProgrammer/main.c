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
uint16_t devId = 0;

struct request request;
uint8_t requestBytesPos;
uint8_t requestBytesLen;

int main(void) {
	// SMPS enable pin as output
	smps_init();

	// Init Padauk pins
	padauk_init();

	// Initialize USB
	usbInit();

	// Force USB re-enumeration
	usbDeviceDisconnect();
	_delay_ms(250);
	usbDeviceConnect();

	sei();

    /* Replace with your application code */
    while (1) {
		usbPoll();
    }
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t* rq = (void*) data;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_VENDOR) {
		return 0;
	}

	requestBytesPos = 0;
	requestBytesLen = (uint8_t) rq->wLength.word;

	switch (rq->bRequest) {
		case CMD_END:
			if (
					(rq->bmRequestType & USBRQ_DIR_MASK) == USBRQ_DIR_DEVICE_TO_HOST &&
					requestBytesLen == 0
			) {
				//if (devId != 0) {
					padauk_finish();
				//}
			}
			break;

		case CMD_START:
			// No data expected
			if (
					(rq->bmRequestType & USBRQ_DIR_MASK) == USBRQ_DIR_DEVICE_TO_HOST &&
					requestBytesLen == 2
			) {
				if (devId != 0) {
					padauk_finish();
				}
				devId = padauk_start(rq->wValue.bytes[0]);
				usbMsgPtr = (uint8_t *) &devId;
				return sizeof(devId);
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

	return 1;
}
