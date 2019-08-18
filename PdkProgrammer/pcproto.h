/*
 * pcproto.h
 *
 * Created: 04/08/2019 23:51:14
 *  Author: Marcos
 */ 


#ifndef PCPROTO_H_
#define PCPROTO_H_

enum {
	CMD_END = 0,
	CMD_START,
	CMD_READ,
	CMD_WRITE,
	CMD_ERASE,
};

struct request_read {
	uint16_t address;
	uint8_t count;
};

struct request_write {
	uint16_t address;
	uint16_t data[32];
};

struct request {
	union {
		struct request_read read;
		struct request_write write;
	};
};

#endif /* PCPROTO_H_ */
