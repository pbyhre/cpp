#include "data_structures/ring_buffer.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_BUFFER_SIZE 16

static void test_init(void) {
    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;

    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    assert(ring_buffer_is_empty(&rb));
    assert(ring_buffer_available(&rb) == 0);
    assert(ring_buffer_free(&rb) == TEST_BUFFER_SIZE - 1);

    printf("test_init: PASSED\n");
}

static void test_write_read(void) {
    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;
    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    const uint8_t data[] = {1, 2, 3, 4, 5};
    size_t written = ring_buffer_write(&rb, data, sizeof(data));

    assert(written == sizeof(data));
    assert(ring_buffer_available(&rb) == sizeof(data));

    uint8_t output[5];
    size_t read = ring_buffer_read(&rb, output, sizeof(output));

    assert(read == sizeof(data));
    assert(memcmp(data, output, sizeof(data)) == 0);
    assert(ring_buffer_is_empty(&rb));

    printf("test_write_read: PASSED\n");
}

static void test_wrap_around(void) {
    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;
    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    uint8_t data[TEST_BUFFER_SIZE - 1];
    for (size_t i = 0; i < sizeof(data); i++) {
        data[i] = (uint8_t)i;
    }

    ring_buffer_write(&rb, data, sizeof(data));

    uint8_t output[TEST_BUFFER_SIZE - 1];
    ring_buffer_read(&rb, output, 8);

    ring_buffer_write(&rb, data, 8);

    assert(ring_buffer_available(&rb) == 15);

    printf("test_wrap_around: PASSED\n");
}

static void test_overflow(void) {
    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;
    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    uint8_t data[TEST_BUFFER_SIZE * 2];
    size_t written = ring_buffer_write(&rb, data, sizeof(data));

    assert(written == TEST_BUFFER_SIZE - 1);

    printf("test_overflow: PASSED\n");
}

int main(void) {
    printf("Running ring_buffer tests...\n");

    test_init();
    test_write_read();
    test_wrap_around();
    test_overflow();

    printf("All tests PASSED\n");
    return 0;
}
