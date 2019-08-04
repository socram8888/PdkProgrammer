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

#define SMPS_EN_DDR DDRA
#define SMPS_EN_PORT PORTA
#define SMPS_EN_BIT 6

static volatile satfrac smpsDuty;
static volatile satfrac smpsAdcTarget;
static volatile satfrac smpsIntegral;
static volatile satfrac smpsError;

void smps_init() {
	// Set PWM pin as output
	DDRA |= _BV(7);

	// Set SMPS enable pin as output
	SMPS_EN_DDR |= _BV(SMPS_EN_BIT);

	// Ref to internal reference and set mux to source 2
	ADMUX = _BV(REFS1) | _BV(MUX1);

	// Disable digital inputs in ADC2
	DIDR0 = _BV(2);
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

	// Enable PWM on OC0B pin
	TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);

	// Set timer 0 clock source to fcpu
	TCCR0B = _BV(CS00);

	// Start with smallest voltage and we'll scale
	OCR0B = 0;
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

	// Disable PWM clock
	TCCR0B = 0x00;

	// Disable PWM pin
	TCCR0A = 0x00;

	smps_wait_target();

	// Now disable ADC to save power
	ADCSRA = 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmispelled-isr"
void __attribute__((signal)) smps_update() {
	satfrac read = satfrac_from_int(ADCW, 1024);
	satfrac error = satfrac_sub(smpsAdcTarget, read);
	smpsError = error;

	smpsIntegral = satfrac_add(error, smpsIntegral);
	
	satfrac correction = satfrac_add(
		satfrac_mul(error, satfrac_from_float(0.5)),
		satfrac_mul(smpsIntegral, satfrac_from_float(1.0 / 256))
	);

	smpsDuty = satfrac_add(smpsDuty, correction);
	OCR0B = min((uint8_t) satfrac_to_int(smpsDuty, 256), 128);
}
#pragma GCC diagnostic pop

ISR(ADC_vect, ISR_NAKED) {
	asm(
		"sei\n" // Re-enable interrupts ASAP
		"rcall smps_update\n" // Recalculate
		"cli\n" // Disable interrupts before setting ADSC, so this does never fire before fully popping all data
		"sbi 0x06, 6\n" // Set ADSC again
		"reti\n"
	);
}
