#include <memory.h>
#include "rbuff.h"

typedef struct RingBuffer
{
    void *array;
    size_t capacity;
    size_t size;

    size_t write;
    size_t read;
} RingBuffer;

RingBuffer *create_ringbuffer(size_t capacity, size_t size)
{
    RingBuffer *rbuff = malloc(sizeof(RingBuffer));

    rbuff->capacity = capacity;
    rbuff->size = size;

    rbuff->array = malloc(capacity * size);
    rbuff->write = 0;
    rbuff->read = 0;

    return rbuff;
}
void destroy_ringbuffer(RingBuffer **rbuff)
{
    if ((*rbuff)->array)
        free((*rbuff)->array);

    free(*rbuff);
    *rbuff = NULL;
}

void *read(RingBuffer *rbuff)
{
    void *item = rbuff->array + rbuff->read;

    if (rbuff->read >= (rbuff->capacity - 1) * rbuff->size)
        rbuff->read = 0;
    else
        rbuff->read += rbuff->size;

    return item;
}
void write(RingBuffer *rbuff, void *item)
{
    memcpy(rbuff->array + rbuff->write, item, rbuff->size);

    if (rbuff->write >= (rbuff->capacity - 1) * rbuff->size)
        rbuff->write = 0;
    else
        rbuff->write += rbuff->size;
}
void clear(RingBuffer *rbuff)
{
    rbuff->read = 0;
    rbuff->write = 0;
}

bool empty(RingBuffer *rbuff)
{
    return rbuff->read == rbuff->write;
}
bool full(RingBuffer *rbuff)
{
    return ((rbuff->write + rbuff->size) % (rbuff->capacity * rbuff->size)) == rbuff->read;
}
