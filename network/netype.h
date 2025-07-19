#ifndef __NETYPE_H__
#define __NETYPE_H__

#include <stdint.h>
#include <stdbool.h>

#include "buffer.h"

typedef uint32_t ip4_addr;

typedef struct {
	ip4_addr	src;
	ip4_addr	dst;
	buffer_t* data;
}packet_t;

packet_t* pkt_create(ip4_addr src, ip4_addr dst, buffer_t* data);
bool			pkt_free(packet_t* self);

#define IP4_STR_SIZE 4*4
ip4_addr	ip4_stoa(const char* ipstr);
void			ip4_atos(ip4_addr addr, char* buf, uint8_t size);


#endif //__NETYPE_H__
