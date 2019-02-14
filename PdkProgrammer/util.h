/*
 * util.h
 *
 * Created: 12/02/2019 17:27:10
 *  Author: Marcos
 */ 

#ifndef UTIL_H_
#define UTIL_H_

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

#endif /* UTIL_H_ */
