#include "queue.h"
#include <stdlib.h>
#include "log.h"

static const char* MM = "QUE";

bool _que_is_null(queue_t* self);

inline bool _que_is_null(queue_t* self) {
	return (self == NULL);
}

queue_t* que_create(bool is_mutex) {
	queue_t* self = calloc(1, sizeof(queue_t));
	self->llt = llt_create();
	self->is_mutex = is_mutex;

	if(self->is_mutex) {
		pthread_mutex_init(&self->mutex, NULL);
		pthread_cond_init(&self->cond, NULL);
	}

	return self;
}

bool que_free(queue_t* self) {
	if(_que_is_null(self)) {
		logging(LL_ERR, MM, "queue pointer is NULL?!");
		return false;
	}
	else if(!llt_free(self->llt)) {
		logging(LL_ERR, MM, "Failed to free queue list");
		return false;
	}

	free(self);
	return true;
}

bool que_enque(queue_t* self, void* data) {
	if(_que_is_null(self)) {
		logging(LL_ERR, MM, "queue pointer is NULL?!");
		return false;
	}

	if(self->is_mutex)
		pthread_mutex_lock(&self->mutex);

	bool ret = llt_insert_at_first(self->llt, data);

	if(self->is_mutex) {
		if(ret)
			pthread_cond_signal(&self->cond);
		pthread_mutex_unlock(&self->mutex);
	}

	return ret;
}

void* que_deque(queue_t* self) {
	if(_que_is_null(self)) {
		logging(LL_ERR, MM, "queue pointer is NULL?!");
		return NULL;
	}

	if(self->is_mutex) {
		pthread_mutex_lock(&self->mutex);
		while(llt_is_empty(self->llt))
			pthread_cond_wait(&self->cond, &self->mutex);
	}

	void* ret = llt_delete_at_last(self->llt);

	if(self->is_mutex)
		pthread_mutex_unlock(&self->mutex);

	return ret;
}
