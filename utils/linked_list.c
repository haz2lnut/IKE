#include "linked_list.h"
#include <stdlib.h>

typedef struct _llt_node_t _llt_node_t;
struct _llt_node_t {
	void*					data;
	_llt_node_t*	prev;
	_llt_node_t*	next;
};
_llt_node_t* _llt_node_create(void* data);
bool _llt_is_null(linked_list_t* self);
bool _llt_is_wrong(linked_list_t* self, uint8_t position);

_llt_node_t* _llt_node_create(void* data) {
	_llt_node_t* self = calloc(1, sizeof(_llt_node_t));
	self->data = data;

	return self;
}

inline bool _llt_is_null(linked_list_t* self) {
	return (self == NULL);
}

inline bool _llt_is_wrong(linked_list_t* self, uint8_t position) {
	return (position > self->size);
}

linked_list_t* llt_create() {
	linked_list_t* self = calloc(1, sizeof(linked_list_t));

	return self;
}

bool llt_free(linked_list_t* self) {
	// free nodes
	_llt_node_t* cur = self->first;
	while(cur != NULL) {
		_llt_node_t* tmp = cur;
		cur = cur->next;
		free(tmp->data);
		free(tmp);
	}

	free(self);
	return true;
}

bool llt_insert(linked_list_t* self, void* data, uint8_t position) {
	// Errors
	if(_llt_is_null(self)) {
		// Logging
		return false;
	}
	else if(!llt_is_empty(self) && _llt_is_wrong(self, position)) {
		// Logging
		return false;
	}

	_llt_node_t* node = _llt_node_create(data);
	_llt_node_t* cur = NULL;

	// Empty list
	if(llt_is_empty(self)) {
		self->first = self->last = node;
	}
	// At last
	else if(position == 0) {
		cur = self->last;
		cur->next = node;
		node->prev = cur;
		self->last = node;
	}
	// At first
	else if(position == 1) {
		cur = self->first;
		node->next = cur;
		cur->prev = node;
		self->first = node;
	}
	// At position
	else {
		cur = self->first;
		for(int i = 2; i < position; i++) cur = cur->next;
		node->prev = cur;
		node->next = cur->next;
		cur->next->prev = node;
		cur->next = node;
	}
	
	self->size++;
	return true;
}

void* llt_delete(linked_list_t* self, uint8_t position) {
	// Errors
	if(_llt_is_null(self)) {
		// Logging
		return NULL;
	}
	else if(llt_is_empty(self)) {
		// Logging
		return NULL;
	}
	else if(_llt_is_wrong(self, position)) {
		// Logging
		return NULL;
	}

	_llt_node_t* cur = NULL;

	// Only one
	if(self->size == 1) {
		cur = self->first;
		self->first = self->last = NULL;
	}
	// At last
	else if(position == 0) {
		cur = self->last;
		self->last = cur->prev;
		((_llt_node_t*)self->last)->next = NULL;
	}
	// At first
	else if(position == 1) {
		cur = self->first;
		self->first = cur->next;
		((_llt_node_t*)self->first)->prev = NULL;
	}
	// At position
	else {
		cur = self->first;
		for(int i = 1; i < position; i++) cur = cur->next;
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
	}
	
	void* ret = cur->data;
	free(cur);
	self->size--;
	return ret;
}

void* llt_travel(linked_list_t* self) {
	// Errors
	if(_llt_is_null(self)) {
		// Logging
		return NULL;
	}
	else if(llt_is_empty(self)) {
		// Logging
		return NULL;
	}

	void* ret = NULL;
	if(self->current) {
		_llt_node_t* node = self->current;
		ret = node->data;
		self->current = node->next;
	}

	return ret;
}

bool llt_travel_reset(linked_list_t* self) {
	if(_llt_is_null(self)) {
		// Logging
		return false;
	}

	self->current = self->first;
	return true;
}
