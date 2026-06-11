#include "data_structures/ring_buffer.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("Ring Buffer Example\n");
    printf("===================\n\n");

    // Allocate backing storage
    uint8_t backing[128];
    ring_buffer_t rb;

    // Initialize ring buffer
    ring_buffer_init(&rb, backing, sizeof(backing));
    printf("Initialized ring buffer with capacity: %zu bytes\n", sizeof(backing));
    printf("Initial free space: %zu bytes\n\n", ring_buffer_free(&rb));

    // Write some data
    const char *message = "Hello, embedded world!";
    size_t written = ring_buffer_write(&rb, (const uint8_t *)message, strlen(message));
    printf("Wrote %zu bytes: \"%s\"\n", written, message);
    printf("Available for reading: %zu bytes\n", ring_buffer_available(&rb));
    printf("Free space: %zu bytes\n\n", ring_buffer_free(&rb));

    // Read data back
    char output[128];
    size_t read = ring_buffer_read(&rb, (uint8_t *)output, written);
    output[read] = '\0';
    printf("Read %zu bytes: \"%s\"\n", read, output);
    printf("Buffer is empty: %s\n\n", ring_buffer_is_empty(&rb) ? "yes" : "no");

    // Demonstrate wrap-around
    printf("Demonstrating wrap-around behavior:\n");
    for (int i = 0; i < 10; i++) {
        char data[20];
        snprintf(data, sizeof(data), "Message %d", i);
        ring_buffer_write(&rb, (const uint8_t *)data, strlen(data) + 1);

        char result[20];
        ring_buffer_read(&rb, (uint8_t *)result, strlen(data) + 1);
        printf("  %s\n", result);
    }

    printf("\nExample completed successfully!\n");
    return 0;
}
