#include "netype.h"
#include <stdlib.h>
#include <stdio.h>
#include "log.h"

static const char* MM = "NET";

packet_t* pkt_create(ip4_addr src, ip4_addr dst, buffer_t* data) {
	packet_t* self = calloc(1, sizeof(packet_t));
	self->src = src;
	self->dst = dst;
	self->data = data;

	return self;
}

bool pkt_free(packet_t* self) {
	buf_free(self->data);
	free(self);
	return true;
}

ip4_addr ip4_stoa(const char* ipstr) {
	unsigned int a,b,c,d;
	if(sscanf(ipstr, "%u.%u.%u.%u", &a,&b,&c,&d) == 4)
		return a + (b<<8) + (c<<16) + (d<<24);

	return 0;
}

void ip4_atos(ip4_addr addr, char* buf, uint8_t size) {
	if(size < IP4_STR_SIZE) {
		logging(LL_ERR, MM, "Wrong ip size(%d)", size);
		return;
	}

	snprintf(buf, IP4_STR_SIZE, "%u.%u.%u.%u",
			addr & 0xFF,
			(addr>>8) & 0xFF,
			(addr>>16) & 0xFF,
			(addr>>24) & 0xFF
			);
}
