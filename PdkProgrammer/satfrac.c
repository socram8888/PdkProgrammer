/*
 * satfrac.c
 *
 * Created: 10/02/2019 13:39:57
 *  Author: Marcos
 */ 

#include "satfrac.h"
#include <stdbool.h>

// Adapted from https://locklessinc.com/articles/sat_arithmetic/

#define SAME_SIGN(a, b) (!((uint8_t) (((a) >> 8 ^ (b) >> 8) & 0x80)))
#define SIGN_BYTE(a) ((uint8_t) ((a) >> 8) & 0x80)

satfrac satfrac_add(satfrac a, satfrac b) {
	satfrac x = a + b;

	/* Only if same sign they can overflow */
	if (!SAME_SIGN(a, b)) {
		return x;
	}

	/* Is the result a different sign? */
	if (SAME_SIGN(a, x)) {
		return x;
	}

	if (SIGN_BYTE(a)) {
		return SATFRAC_MIN;
	} else {
		return SATFRAC_MAX;
	}
}

satfrac satfrac_sub(satfrac a, satfrac b) {
	satfrac x = a - b;

	/* Only if different sign they can overflow */
	if (SAME_SIGN(a, b)) {
		return x;
	}

	/* Is the result a different sign? */
	if (SAME_SIGN(a, x)) {
		return x;
	}

	if (SIGN_BYTE(a)) {
		return SATFRAC_MIN;
	} else {
		return SATFRAC_MAX;
	}
}

satfrac satfrac_mul(satfrac a, satfrac b) {
	uint16_t res = 0;

	if (SIGN_BYTE(b)) {
		a = -a;
		b = -b;
	}

	b = b << 1;
	while (b) {
		a = a >> 1;
		if (SIGN_BYTE(b)) {
			res += a;
		}
		b = b << 1;
	}

	return res;
}
