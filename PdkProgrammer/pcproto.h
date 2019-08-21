/*
 * pcproto.h
 *
 * Created: 04/08/2019 23:51:14
 *  Author: Marcos
 */ 


#ifndef PCPROTO_H_
#define PCPROTO_H_

enum {
	CMD_MODE = 0,
	CMD_READ,
	CMD_WRITE,
	CMD_ERASE
};

enum {
	MODE_OFF = 0,
	MODE_READ,
	MODE_WRITE,
	MODE_ERASE
};

#endif /* PCPROTO_H_ */
