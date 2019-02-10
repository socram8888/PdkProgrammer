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
#define IS_NEG(a) ((uint8_t) ((a) >> 8) & 0x80)

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

	if (IS_NEG(a)) {
		return INT16_MIN;
	} else {
		return INT16_MAX;
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

	if (IS_NEG(a)) {
		return INT16_MIN;
	} else {
		return INT16_MAX;
	}
}

satfrac satfrac_mul(satfrac a, satfrac b) {
	bool neg = !SAME_SIGN(a, b);
	uint16_t ua = a & 0x7FFF;

	uint16_t res = 0;
	b = b << 1;
	while (b) {
		ua = ua >> 1;
		if (b & 0x8000) {
			res += ua;
		}
		b = b << 1;
	}

	if (neg) {
		res |= 0x8000;
	}
	return res;
}
