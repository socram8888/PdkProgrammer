/*
 * smps.h
 *
 * Created: 12/02/2019 17:26:01
 *  Author: Marcos
 */ 

#ifndef SMPS_H_
#define SMPS_H_

#include "satfrac.h"

// Voltage divisor value
#define SMPS_R1 680.0
#define SMPS_R2 68.0
#define SMPS_REF 1.1

static inline satfrac smps_adc_target(float volts) {
	return satfrac_from_float(volts * SMPS_R2 / (SMPS_R1 + SMPS_R2) / SMPS_REF);
}

void smps_init();
void smps_on(satfrac adcTarget);
void smps_switch(satfrac adcTarget);
void smps_off();

#endif /* SMPS_H_ */
