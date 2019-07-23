#ifndef __RINGBUFFER_HEADER_INCLUDED__
#define __RINGBUFFER_HEADER_INCLUDED__

typedef struct {
  char *buffer;
  size_t size;
  size_t reader;
  size_t writer;
} RingBuffer;

RingBuffer *new_RingBuffer(size_t size);
void free_RingBuffer(RingBuffer **rb_ptr);
size_t readblelen_RingBuffer(RingBuffer *rb);
char *read_RingBuffer(RingBuffer *rb);
void write_RingBuffer(RingBuffer *rb, char *str, size_t size);

#endif
