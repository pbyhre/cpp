#include "data_structures/ring_buffer.h"
#include <string.h>

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, size_t capacity) {
    rb->buffer = buffer;
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
}

size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len) {
    size_t available = ring_buffer_free(rb);
    if (len > available) {
        len = available;
    }

    size_t first_chunk = rb->capacity - rb->head;
    if (len <= first_chunk) {
        memcpy(rb->buffer + rb->head, data, len);
        rb->head = (rb->head + len) % rb->capacity;
    } else {
        memcpy(rb->buffer + rb->head, data, first_chunk);
        memcpy(rb->buffer, data + first_chunk, len - first_chunk);
        rb->head = len - first_chunk;
    }

    return len;
}

size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, size_t len) {
    size_t available = ring_buffer_available(rb);
    if (len > available) {
        len = available;
    }

    size_t first_chunk = rb->capacity - rb->tail;
    if (len <= first_chunk) {
        memcpy(data, rb->buffer + rb->tail, len);
        rb->tail = (rb->tail + len) % rb->capacity;
    } else {
        memcpy(data, rb->buffer + rb->tail, first_chunk);
        memcpy(data + first_chunk, rb->buffer, len - first_chunk);
        rb->tail = len - first_chunk;
    }

    return len;
}

size_t ring_buffer_available(const ring_buffer_t *rb) {
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    }
    return rb->capacity - rb->tail + rb->head;
}

size_t ring_buffer_free(const ring_buffer_t *rb) {
    return rb->capacity - ring_buffer_available(rb) - 1;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    return rb->head == rb->tail;
}

void ring_buffer_clear(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
}
