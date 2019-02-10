/*
 * satfrac.h
 *
 * Created: 10/02/2019 13:18:05
 *  Author: Marcos
 */ 


#ifndef SATFRAC_H_
#define SATFRAC_H_

#include <stdint.h>

typedef int16_t satfrac;

static inline satfrac satfrac_from_float(float value) {
	return (satfrac) (32768 * value);
}

static inline satfrac satfrac_from_int(int16_t value, uint16_t maxValue) {
	return (satfrac) ((int32_t) 32768 * value / maxValue);
}

static inline int16_t satfrac_to_int(satfrac frac, uint16_t maxValue) {
	return (int16_t) ((int32_t) frac * maxValue / 32768);
}

static inline float satfrac_to_float(satfrac value) {
	return value / 32768.0;
}

satfrac satfrac_add(satfrac a, satfrac b);
satfrac satfrac_sub(satfrac a, satfrac b);
satfrac satfrac_mul(satfrac a, satfrac b);

#endif /* SATFRAC_H_ */
