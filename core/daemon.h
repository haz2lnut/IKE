#ifndef __DAEMON_H__
#define __DAEMON_H__

#include "network.h"
#include "queue.h"
#include "log.h"

#define WORKER_MAX 10

typedef struct {
	network_t* NET;
	log_t* LOG;

	bool			is_running;
	pthread_t worker[WORKER_MAX];
	queue_t*	job_que;
}daemon_t;

typedef struct {
	void*			(*func)(void*);
	void*			arg;
}job_t;

extern daemon_t DAEMON;

bool daemon_create();
bool daemon_free();
bool running();
bool push_job(void* (*func)(void*), void* arg);

#endif //__DAEMON_H__
