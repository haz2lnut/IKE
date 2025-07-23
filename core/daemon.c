#include "daemon.h"
#include <stdlib.h>
#include <signal.h>

daemon_t DAEMON;
static const char* MM = "DMN";

bool _daemon_close() {
	DAEMON.is_running = false;

	// Close modules
	if(!net_close(DAEMON.NET)) {
		logging(LL_ERR, MM, "Failed to close network module");
		return false;
	}

	// Close workers
	for(int i = 0; i < WORKER_MAX; i++) {
		if(!que_enque(DAEMON.job_que, NULL)) {
			logging(LL_ERR, MM, "Failed to close worker(%d)", i);
			return false;
		}
	}

	for(int i = 0; i < WORKER_MAX; i++)
		pthread_join(DAEMON.worker[i], NULL);

	logging(LL_INFO, MM, "Close DAEMON");
	return true;
}

void _signal_handler(int sig) {
	switch(sig) {
		case SIGINT:
			_daemon_close();
			break;
	}
}

void* _worker_loop(void* arg) {
	int id = *(int*)arg;
	free(arg);
	while(DAEMON.is_running) {
		job_t* job = que_deque(DAEMON.job_que);
		if(job == NULL)
			break;
		else if(job->func) {
			job->func(job->arg);
			free(job);
		}
	}

	logging(LL_INFO, MM, "Close worker[%d]", id);
	return NULL;
}

bool daemon_create() {
	if((DAEMON.LOG = log_create()) == NULL) {
		return false;
	}

	if((DAEMON.job_que = que_create(true)) == NULL) {
		logging(LL_ERR, MM, "Failed to create joq_que");
		return false;
	}

	if((DAEMON.NET = net_create()) == NULL) {
		logging(LL_ERR, MM, "Failed to create notwork module");
		return false;
	}

	logging(LL_INFO, MM, "DAEMON created");
	return true;
}

bool daemon_free() {
	if(!DAEMON.is_running) {
		if(!que_free(DAEMON.job_que)) {
			logging(LL_ERR, MM, "Failed to free joq_que");
			return false;
		}
		else if(!net_free(DAEMON.NET)) {
			logging(LL_ERR, MM, "Failed to free notwork module");
			return false;
		}
		logging(LL_INFO, MM, "Free DAEMON");
		return true;
	}
	else {
		logging(LL_ERR, MM, "Failed to free daemon, because it's not running");
		return false;
	}
}

bool running() {
	DAEMON.is_running = true;

	signal(SIGINT, _signal_handler);

	for(int i = 0; i < WORKER_MAX; i++) {
		int* arg = calloc(1, sizeof(int));
		*arg = i;
		if(pthread_create(&DAEMON.worker[i], NULL, _worker_loop, arg) != 0) {
			logging(LL_ERR, MM, "Failed to free daemon, because it's not running");
			_daemon_close();
			return false;
		}
		else {
			logging(LL_INFO, MM, "Create worker[%d]", i);
		}
	}

	for(int i = 0; i < WORKER_MAX; i++)
		pthread_join(DAEMON.worker[i], NULL);

	return true;
}

bool push_job(void* (*func)(void*), void* arg) {
	job_t* job = calloc(1, sizeof(job_t));
	job->func = func;
	job->arg = arg;
	if(!que_enque(DAEMON.job_que, job)) {
		logging(LL_ERR, MM, "Failed to push job");
		return false;
	}
	return true;
}
