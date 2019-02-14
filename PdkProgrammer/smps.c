/*
 * smps.c
 *
 * Created: 12/02/2019 17:19:02
 *  Author: Marcos
 */ 

#include "smps.h"
#include "util.h"
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define SMPS_EN_DDR DDRD
#define SMPS_EN_PORT PORTD
#define SMPS_EN_BIT 4

static volatile satfrac smpsDuty;
static volatile satfrac smpsAdcTarget;
static volatile satfrac smpsIntegral;
static volatile satfrac smpsError;

void smps_init() {
	// Set timer 0 clock source to fcpu
	TCCR0B = _BV(CS00);

	// Set PWM pin as output
	DDRD |= _BV(6);

	// Set SMPS enable pin as output
	SMPS_EN_DDR |= _BV(SMPS_EN_BIT);

	// Ref to internal reference and set mux to source 0
	ADMUX = _BV(REFS1) | _BV(REFS0);

	// Disable digital inputs in ADC0
	DIDR0 = _BV(0);
}

void smps_wait_target() {
	smpsError = SATFRAC_MAX;

	// Wait until error is less than 5%
	while (satfrac_abs(smpsError) > satfrac_from_float(0.05));
}

void smps_on(satfrac adcTarget) {
	smpsAdcTarget = adcTarget;
	smpsIntegral = satfrac_from_float(0);

	// Enable SMPS
	SMPS_EN_PORT &= ~_BV(SMPS_EN_BIT);

	// Enable PWM on OC0A pin
	TCCR0A = (TCCR0A & (_BV(COM0B1) | _BV(COM0B0))) | _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);

	// Start with smallest voltage and we'll scale
	OCR0A = 0;
	smpsDuty = 0;

	// Enable ADC, start sampling, enable interrupts and set prescaler to 128
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	// Wait for target
	smps_wait_target();
}

void smps_switch(satfrac adcTarget) {
	smpsAdcTarget = adcTarget;
	smps_wait_target();
}

void smps_off() {
	smpsAdcTarget = 0;
	
	// Disable SMPS
	SMPS_EN_PORT |= _BV(SMPS_EN_BIT);

	// Disable PWM pin
	TCCR0A = 0x00;

	smps_wait_target();

	// Now disable ADC to save power
	ADCSRA = 0;
}

ISR(ADC_vect) {
	// LED pin
	DDRB |= _BV(5);
	PORTB |= _BV(5);

	satfrac read = satfrac_from_int(ADCW, 1024);
	satfrac error = satfrac_sub(smpsAdcTarget, read);
	smpsError = error;

	smpsIntegral = satfrac_add(error, smpsIntegral);
	
	satfrac correction = satfrac_add(
	satfrac_mul(error, satfrac_from_float(0.5)),
	satfrac_mul(smpsIntegral, satfrac_from_float(1.0 / 256))
	);

	smpsDuty = satfrac_add(smpsDuty, correction);
	OCR0A = min((uint8_t) satfrac_to_int(smpsDuty, 256), 128);

	ADCSRA |= _BV(ADSC);
}
