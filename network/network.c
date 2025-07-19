#include <stdlib.h>
#include <arpa/inet.h>
#include "network.h"
#include "daemon.h"

void* _net_receiving(void* arg);
void* _net_sending(void* arg);

network_t* net_create() {
	network_t* self = calloc(1, sizeof(network_t));
	self->recv_que = que_create(true);
	self->send_que = que_create(true);
	self->port = 500;
	self->sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(self->sock < 0) {
		// Logging
		return NULL;
	}

	// pkt info
	int opt = 1;
	setsockopt(self->sock, IPPROTO_IP, IP_PKTINFO, &opt, sizeof(opt));

	// bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(self->port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(self->sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		// Logging
		return NULL;
	}

	return self;
}

bool net_free(network_t* self) {
	if(self && que_free(self->recv_que) && que_free(self->send_que)) {
		free(self);
		return true;
	}
	else {
		// Logging
		return false;
	}
}

bool net_close(network_t* self) {
	if(que_enque(self->recv_que, NULL) && que_enque(self->send_que, NULL)) {
		shutdown(self->sock, SHUT_RD);
		return true;
	}
	return false;
}

void* _net_receiving(void* arg) {
	network_t* self = arg;
	char buf[1024];
	char ancillary[64];

	while(DAEMON.is_running) {
		// Set msghdr
		struct msghdr msg;
		struct sockaddr_in client;
		struct iovec iov;
		msg.msg_name = &client;
		msg.msg_namelen = sizeof(client);
		iov.iov_base = buf;
		iov.iov_len = sizeof(buf);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = ancillary;
		msg.msg_controllen = sizeof(ancillary);
		msg.msg_flags = 0;

		int len = recvmsg(self->sock, &msg, 0);
		if(len > 0) {
			for(struct cmsghdr* cm = CMSG_FIRSTHDR(&msg);
					cm != NULL;
					cm = CMSG_NXTHDR(&msg, cm))
			{
				if(cm->cmsg_level == IPPROTO_IP) {
					struct in_pktinfo* info = (struct in_pktinfo*)CMSG_DATA(cm);
					buffer_t* data = buf_create(len);
					buf_write(data, buf, len);
					packet_t* pkt = pkt_create(
							client.sin_addr.s_addr,
							info->ipi_addr.s_addr,
							data
							);
					que_enque(self->recv_que, pkt);
					break;
				}
			}
		}
		else if(len == -1) {
			// Logging
			break;
		}
	}

	return NULL;
}

void* _net_sending(void* arg) {
	network_t* self = arg;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = self->port;

	while(DAEMON.is_running) {
		packet_t* pkt = que_deque(self->send_que);
		if(pkt == NULL)
			break;

		addr.sin_addr.s_addr = pkt->dst;
		buffer_t* buf = pkt->data;
		sendto(self->sock, buf->data + buf->offset, buf->size, 0,
				(struct sockaddr*)&addr, sizeof(addr));
		pkt_free(pkt);
	}

	return NULL;
}

bool net_send(ip4_addr src, ip4_addr dst, buffer_t* data) {
	network_t* self = DAEMON.NET;
	packet_t* pkt = pkt_create(src, dst, data);

	if(pkt)
		return que_enque(self->send_que, pkt);
	else
		return false;
}

buffer_t* net_recv(ip4_addr* src, ip4_addr* dst) {
	network_t* self = DAEMON.NET;
	packet_t* pkt = que_deque(self->recv_que);

	if(pkt) {
		if(src) *src = pkt->src;
		if(dst) *dst = pkt->dst;
		return pkt->data;
	}

	return NULL;
}
