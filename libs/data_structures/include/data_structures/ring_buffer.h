#ifndef ROBOTICS_DATA_STRUCTURES_RING_BUFFER_H
#define ROBOTICS_DATA_STRUCTURES_RING_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Lock-free single-producer single-consumer ring buffer
 *
 * Designed for embedded systems and real-time applications.
 * No dynamic memory allocation - buffer must be provided by caller.
 */
typedef struct {
    uint8_t *buffer;      /**< Pointer to buffer storage */
    size_t capacity;      /**< Total capacity in bytes */
    volatile size_t head; /**< Write position */
    volatile size_t tail; /**< Read position */
} ring_buffer_t;

/**
 * @brief Initialize a ring buffer
 *
 * @param rb Pointer to ring buffer structure
 * @param buffer Pointer to backing storage (must remain valid)
 * @param capacity Size of backing storage in bytes
 */
void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, size_t capacity);

/**
 * @brief Write data to ring buffer
 *
 * @param rb Pointer to ring buffer
 * @param data Data to write
 * @param len Length of data in bytes
 * @return Number of bytes actually written (may be less than len if full)
 */
size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len);

/**
 * @brief Read data from ring buffer
 *
 * @param rb Pointer to ring buffer
 * @param data Buffer to read into
 * @param len Maximum bytes to read
 * @return Number of bytes actually read
 */
size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, size_t len);

/**
 * @brief Get available bytes for reading
 *
 * @param rb Pointer to ring buffer
 * @return Number of bytes available to read
 */
size_t ring_buffer_available(const ring_buffer_t *rb);

/**
 * @brief Get free space for writing
 *
 * @param rb Pointer to ring buffer
 * @return Number of bytes available for writing
 */
size_t ring_buffer_free(const ring_buffer_t *rb);

/**
 * @brief Check if ring buffer is empty
 *
 * @param rb Pointer to ring buffer
 * @return true if empty
 */
bool ring_buffer_is_empty(const ring_buffer_t *rb);

/**
 * @brief Clear the ring buffer
 *
 * @param rb Pointer to ring buffer
 */
void ring_buffer_clear(ring_buffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTICS_DATA_STRUCTURES_RING_BUFFER_H */
