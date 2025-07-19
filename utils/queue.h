#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "linked_list.h"
#include <pthread.h>

typedef struct {
	linked_list_t* llt;

	// Mutex
	bool						is_mutex;
	pthread_mutex_t mutex;
	pthread_cond_t	cond;
}queue_t;

queue_t*	que_create(bool is_mutex);
bool			que_free(queue_t* self);

bool	que_enque(queue_t* self, void* data);
void* que_deque(queue_t* self);

#endif //__QUEUE_H__
