#ifndef __SA_H__
#define __SA_H__

#include "netype.h"

#define SA_MAX 10

typedef struct {
	struct {
		ip4_addr addr;
	}local, remote;
	char* secret;

	bool is_initiator;
	uint64_t SPIi;
	uint64_t SPIr;
	uint8_t version;
	uint32_t message_id;
	uint32_t last_exchange_type;
}sa_t;

uint8_t sa_create_by_conf(sa_t** sa_entry, const char* filename);

#endif //__SA_H__
