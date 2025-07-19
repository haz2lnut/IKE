#include "buffer.h"
#include <stdlib.h>
#include <string.h>

bool _buf_is_null(buffer_t* self);

inline bool _buf_is_null(buffer_t* self) {
	return (self == NULL);
}

buffer_t* buf_create(uint16_t capacity) {
	buffer_t* self = calloc(1, sizeof(buffer_t));
	self->capacity = capacity;
	self->data = calloc(capacity, sizeof(uint8_t));

	return self;
}

bool buf_free(buffer_t* self) {
	free(self->data);
	free(self);

	return true;
}

bool buf_write_raw(buffer_t* self, void* src, uint16_t size, bool reverse) {
	// Errors
	if(_buf_is_null(self)) {
		// Logging
		return false;
	}
	else if(size == 0) {
		// Logging
		return false;
	}

	// Free up mem
	uint16_t end_of_remains = (self->capacity - self->offset + self->size);
	if(size < end_of_remains) {
		uint16_t begin_of_remains = (self->capacity - self->size + 1);

		// Realloc
		if(size > end_of_remains + begin_of_remains) {
			self->data = realloc(self->data, self->capacity + size);
			self->capacity += size;
		}

		// Relocation
		memmove(self->data, self->data + self->offset, self->size);
		self->offset = 0;
	}

	// Write
	uint8_t* d = self->data + self->offset + self->size;
	if(src == NULL)
		memset(d, 0, size);
	else {
		if(reverse) {
			uint8_t* s = src + size - 1;
			for(int i = 0; i < size; i++) {
				*d = *s;
				d++;
				s--;
			}
		}
		else
			memcpy(d, src, size);
	}

	self->size += size;

	return true;
}
