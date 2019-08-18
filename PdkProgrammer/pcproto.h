/*
 * pcproto.h
 *
 * Created: 04/08/2019 23:51:14
 *  Author: Marcos
 */ 


#ifndef PCPROTO_H_
#define PCPROTO_H_

enum {
	CMD_SET_MODE = 1,
	CMD_READ,
	CMD_ERASE,
	CMD_WRITE
};

enum {
	MODE_OFF = 0,
	MODE_READ,
	MODE_WRITE
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

struct reply_mode {
	uint8_t currentMode;
	uint16_t devId;
};

struct reply {
	union {
		struct reply_mode mode;
	};
};

#endif /* PCPROTO_H_ */
