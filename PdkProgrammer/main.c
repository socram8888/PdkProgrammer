/*
 * PdkProgrammer.c
 *
 * Created: 09/02/2019 15:30:34
 * Author : Marcos
 */ 

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"

#ifndef _BV
#define _BV(x) (1 << x)
#endif

// Voltage divisor value
#define ADCRATIO (2200.0 / 220.0)
#define ADCREF 1.25
#define volt2adc(v) satfrac_from_float(v / ADCRATIO / ADCREF)

#define max(a, b) ((a) > (b) ? (a) : (b))

struct PsuStatus {
	satfrac target;
	satfrac integral;
	satfrac previous_error;
};

struct PsuStatus psuStatus[2] = {0};

void psu_on(uint8_t output, uint8_t adcTarget) {
	if (output == 0) {
		// Enable PWM on OC0A pin
		TCCR0A = (TCCR0A & (_BV(COM0B1) | _BV(COM0B0))) | _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);

		// Start with smallest voltage and we'll scale
		// Disable interrupts to avoid asynchronous ADC samplings from screwing up OCR0A
		cli();
		psuStatus[0].target = adcTarget;
		OCR0A = 0;
		sei();

		if (!(ADCSRA & _BV(ADSC))) {
			// Ref to internal reference and set mux to source 0
			ADMUX = _BV(REFS1) | _BV(REFS0);
			// Start sampling
			ADCSRA |= _BV(ADSC);
		}
	} else {
		// Enable PWM on OC0B pin
		TCCR0A = (TCCR0A & (_BV(COM0A1) | _BV(COM0A0))) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);

		// Start with smallest voltage and we'll scale
		// Disable interrupts to avoid asynchronous ADC samplings from screwing up OCR0B
		cli();
		psuStatus[1].target = adcTarget;
		OCR0B = 1;
		sei();

		// Enable ADC if off
		if (!(ADCSRA & _BV(ADSC))) {
			// Ref to internal reference and set mux to source 1
			ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX0);
			// Enable ADC, start sampling, enable interrupts and set prescaler to 128
			ADCSRA |= _BV(ADSC);
		}
	}
}

void psu_off(uint8_t output) {
	if (output == 0) {
		TCCR0A = TCCR0A & 0b00111111;
		psuStatus[0].target = 0;
	} else {
		TCCR0A = TCCR0A & 0b11001111;
		psuStatus[1].target = 0;
	}
}

void log_nibble_hex(uint8_t val) {
	val &= 0xF;
	if (val >= 0xA) {
		USART_Transmit(val + 'A' - 0xA);
	} else {
		USART_Transmit(val + '0');
	}
}

void log_hex(uint8_t val) {
	log_nibble_hex(val >> 4);
	log_nibble_hex(val);
}

int main(void) {
	// 57600 baud
	USART_Init(16);
	USART_Transmit('H');
	USART_Transmit('i');
	USART_Transmit('\n');

	// Set timer 0 clock source to fcpu
	TCCR0B = _BV(CS00);

	// Set PWM pins as outputs
	PORTD = 0;
	DDRD = _BV(6) | _BV(5);

	// Enable ADC, enable interrupts and set prescaler to 128
	ADCSRA = _BV(ADEN) | _BV(ADIE) | 7 << ADPS0;

	// Disable digital inputs in ADC0 and ADC1
	DIDR0 = _BV(1) | _BV(0);

	psu_on(0, volt2adc(5.8));
	psu_on(1, volt2adc(3.3));

	// LED pin
	DDRB |= _BV(5);
	PORTB |= _BV(5);
	
    /* Replace with your application code */
    while (1) 
    {
	    log_hex(psuStatus[0].target);
	    USART_Transmit(' ');
	    log_hex(psuStatus[1].target);
	    USART_Transmit(' ');
	    log_hex(psuStatus[0].read);
	    USART_Transmit(' ');
	    log_hex(psuStatus[1].read);
		USART_Transmit(' ');
		log_hex(OCR0A);
		USART_Transmit(' ');
		log_hex(OCR0B);
		USART_Transmit('\n');
		_delay_ms(200);
    }
}

ISR(ADC_vect, ISR_NOBLOCK) {
	if (!(ADMUX & _BV(MUX0))) {
		satfrac read = satfrac_from_int(ADCH << 8 | ADCL, 4096);
		psuStatus[0].read = read;
		if (psuStatus[0].target > 0) {
			satfrac error = satfrac_sub(psuStatus[0].target, read);
			psuStatus[0]
			OCR0A = (uint16_t) psuStatus[0].target * (OCR0A + 1) / adcRead[0] - 1;
			if (adcTargets[1] > 0) {
				ADMUX |= _BV(MUX0);
			}
		} else if (adcTargets[1] == 0) {
			return;
		}
	} else {
		psuStatus[1].read = ADCH;
		if (adcTargets[1] > 0) {
			OCR0B = (uint16_t) adcTargets[1] * (OCR0B + 1) / adcRead[1] - 1;
			if (adcTargets[0] > 0) {
				ADMUX &= ~_BV(MUX0);
			}
		} else if (adcTargets[0] == 0) {
			return;
		}
	}
	ADCSRA |= _BV(ADSC);
}
