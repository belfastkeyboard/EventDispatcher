#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct RingBuffer RingBuffer;

RingBuffer *create_ringbuffer(size_t capacity, size_t size);
void destroy_ringbuffer(RingBuffer **rbuff);

// call !empty() before read() to ensure no value is read twice
void *read(RingBuffer *rbuff);
// call !full() before write() to ensure no data is overwritten before being read
void write(RingBuffer *rbuff, void *item);
void clear(RingBuffer *rbuff);

bool empty(RingBuffer *rbuff);
bool full(RingBuffer *rbuff);
