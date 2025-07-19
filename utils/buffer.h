#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>
#include <stdbool.h>

/*
 * 0 1 2 3 4 5 6 7 8
 * +-+-+-+-+-+-+-+-+
 * |   | Filed   | |
 * +-+-+-+-+-+-+-+-+
 * capacity = 8
 * size = 5
 * offset = 2
 */
typedef struct {
	uint8_t* data;
	uint16_t capacity;
	uint16_t size;
	uint16_t offset;
}buffer_t;

buffer_t* buf_create(uint16_t capacity);
bool			buf_free(buffer_t* self);

bool buf_write_raw(buffer_t* self, void* src, uint16_t size, bool reverse);
#define buf_write(self, src, size) buf_write_raw(self, src, size, 0)
#define buf_rwrite(self, src, size) buf_write_raw(self, src, size, 1)

bool buf_read_raw(buffer_t* self, void* dst, uint16_t size, bool reverse);

#endif //__BUFFER_H__
