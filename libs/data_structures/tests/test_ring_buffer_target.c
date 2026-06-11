#include "data_structures/ring_buffer.h"
#include "test_assert.h"

#define TEST_BUFFER_SIZE 16

static void test_init(void) {
    TEST_BEGIN("init");

    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;

    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_EQ(ring_buffer_available(&rb), 0);
    TEST_ASSERT_EQ(ring_buffer_free(&rb), TEST_BUFFER_SIZE - 1);

    TEST_END();
}

static void test_write_read(void) {
    TEST_BEGIN("write_read");

    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;
    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    const uint8_t data[] = {1, 2, 3, 4, 5};
    size_t written = ring_buffer_write(&rb, data, sizeof(data));

    TEST_ASSERT_EQ(written, sizeof(data));
    TEST_ASSERT_EQ(ring_buffer_available(&rb), sizeof(data));

    uint8_t output[5];
    size_t read = ring_buffer_read(&rb, output, sizeof(output));

    TEST_ASSERT_EQ(read, sizeof(data));
    TEST_ASSERT_MEM_EQ(data, output, sizeof(data));
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));

    TEST_END();
}

static void test_wrap_around(void) {
    TEST_BEGIN("wrap_around");

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

    TEST_ASSERT_EQ(ring_buffer_available(&rb), 15);

    TEST_END();
}

static void test_overflow(void) {
    TEST_BEGIN("overflow");

    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;
    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    uint8_t data[TEST_BUFFER_SIZE * 2];
    size_t written = ring_buffer_write(&rb, data, sizeof(data));

    TEST_ASSERT_EQ(written, TEST_BUFFER_SIZE - 1);

    TEST_END();
}

static void test_clear(void) {
    TEST_BEGIN("clear");

    uint8_t backing[TEST_BUFFER_SIZE];
    ring_buffer_t rb;
    ring_buffer_init(&rb, backing, TEST_BUFFER_SIZE);

    const uint8_t data[] = {1, 2, 3, 4, 5};
    ring_buffer_write(&rb, data, sizeof(data));

    TEST_ASSERT_FALSE(ring_buffer_is_empty(&rb));

    ring_buffer_clear(&rb);

    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_EQ(ring_buffer_available(&rb), 0);

    TEST_END();
}

int main(void) {
    TEST_INIT();

    test_init();
    test_write_read();
    test_wrap_around();
    test_overflow();
    test_clear();

    return TEST_REPORT();
}
