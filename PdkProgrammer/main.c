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

uint8_t requestBytesPos;
uint8_t requestBytesLen;

union {
	uint16_t wordBuffer[127];
	uint16_t devId;
} iobuf;

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

	switch (rq->bRequest) {
		case CMD_MODE:
			// No data expected
			if (
					(rq->bmRequestType & USBRQ_DIR_MASK) == USBRQ_DIR_DEVICE_TO_HOST &&
					rq->wLength.word >= 2
			) {
				if (currentMode != MODE_OFF) {
					padauk_finish();
					currentMode = MODE_OFF;
				}

				iobuf.devId = 0;
				switch (rq->wValue.bytes[0]) {
					case MODE_READ:
						iobuf.devId = padauk_start(0x06);
						break;

					case MODE_WRITE:
						iobuf.devId = padauk_start(0x07);
						break;

					case MODE_ERASE:
						iobuf.devId = padauk_start(0x03);
						break;
				}

				if (iobuf.devId != 0x0000) {
					currentMode = rq->wValue.bytes[0];
				}

				usbMsgPtr = (uint8_t *) &iobuf.devId;
				return sizeof(iobuf.devId);
			}
			break;

		case CMD_READ:
			if (rq->wLength.word > sizeof(iobuf.wordBuffer)) {
				requestBytesLen = sizeof(iobuf.wordBuffer);
			} else {
				// Round to nearest word
				requestBytesLen = (uint8_t) rq->wLength.word & 0xFE;
			}

			if (currentMode != MODE_READ) {
				for (uint8_t offset = 0; offset < requestBytesLen / 2; offset++) {
					iobuf.wordBuffer[offset] = 0xDEAD;
				}
			} else {
				for (uint8_t offset = 0; offset < requestBytesLen / 2; offset++) {
					iobuf.wordBuffer[offset] = padauk_flash_read(rq->wIndex.word + offset);
				}
			}

			usbMsgPtr = (uint8_t *) iobuf.wordBuffer;
			return requestBytesLen;

		case CMD_WRITE:
			if (
				requestBytesLen >= 4 && // At least one word to write
				requestBytesLen % 2 == 1 && // Only pairs of bytes
				requestBytesLen < sizeof(iobuf.wordBuffer) // Do not exceed buffer size
			) {
				currentCommand = CMD_WRITE;
				return USB_NO_MSG;
			}
			break;
	}

	return 0;
}

USB_PUBLIC uchar usbFunctionWrite(uchar * data, usbMsgLen_t len) {
	/*
	uint8_t * requestBytes = (uint8_t *) &request;
	for (uint8_t i = 0; i < len; i++) {
		requestBytes[requestBytesPos + i] = data[i];
	}
	requestBytesPos += len;

	if (requestBytesPos < requestBytesLen) {
		return 0;
	}
*/
	return 1;
}
