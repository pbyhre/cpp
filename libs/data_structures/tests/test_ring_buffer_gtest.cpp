#include "data_structures/ring_buffer.h"
#include <gtest/gtest.h>
#include <cstring>

class RingBufferTest : public ::testing::Test {
protected:
    static constexpr size_t BUFFER_SIZE = 16;
    uint8_t backing[BUFFER_SIZE];
    ring_buffer_t rb;

    void SetUp() override {
        ring_buffer_init(&rb, backing, BUFFER_SIZE);
    }
};

TEST_F(RingBufferTest, InitializesCorrectly) {
    EXPECT_TRUE(ring_buffer_is_empty(&rb));
    EXPECT_EQ(ring_buffer_available(&rb), 0);
    EXPECT_EQ(ring_buffer_free(&rb), BUFFER_SIZE - 1);
}

TEST_F(RingBufferTest, WriteAndRead) {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    size_t written = ring_buffer_write(&rb, data, sizeof(data));

    EXPECT_EQ(written, sizeof(data));
    EXPECT_EQ(ring_buffer_available(&rb), sizeof(data));

    uint8_t output[5];
    size_t read = ring_buffer_read(&rb, output, sizeof(output));

    EXPECT_EQ(read, sizeof(data));
    EXPECT_EQ(memcmp(data, output, sizeof(data)), 0);
    EXPECT_TRUE(ring_buffer_is_empty(&rb));
}

TEST_F(RingBufferTest, WrapAround) {
    // Fill buffer almost to capacity
    uint8_t data[BUFFER_SIZE - 1];
    for (size_t i = 0; i < sizeof(data); i++) {
        data[i] = static_cast<uint8_t>(i);
    }

    ring_buffer_write(&rb, data, sizeof(data));

    // Read part of it
    uint8_t output[8];
    ring_buffer_read(&rb, output, sizeof(output));

    // Write more, causing wrap-around
    ring_buffer_write(&rb, data, 8);

    EXPECT_EQ(ring_buffer_available(&rb), 15);
}

TEST_F(RingBufferTest, OverflowHandling) {
    uint8_t data[BUFFER_SIZE * 2];
    size_t written = ring_buffer_write(&rb, data, sizeof(data));

    // Should only write up to capacity - 1
    EXPECT_EQ(written, BUFFER_SIZE - 1);
    EXPECT_EQ(ring_buffer_free(&rb), 0);
}

TEST_F(RingBufferTest, Clear) {
    const uint8_t data[] = {1, 2, 3, 4, 5};
    ring_buffer_write(&rb, data, sizeof(data));

    EXPECT_FALSE(ring_buffer_is_empty(&rb));

    ring_buffer_clear(&rb);

    EXPECT_TRUE(ring_buffer_is_empty(&rb));
    EXPECT_EQ(ring_buffer_available(&rb), 0);
    EXPECT_EQ(ring_buffer_free(&rb), BUFFER_SIZE - 1);
}

TEST_F(RingBufferTest, PartialRead) {
    const uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    ring_buffer_write(&rb, data, sizeof(data));

    uint8_t output[4];
    size_t read = ring_buffer_read(&rb, output, sizeof(output));

    EXPECT_EQ(read, 4);
    EXPECT_EQ(ring_buffer_available(&rb), 4);
    EXPECT_FALSE(ring_buffer_is_empty(&rb));
}

TEST_F(RingBufferTest, ReadMoreThanAvailable) {
    const uint8_t data[] = {1, 2, 3};
    ring_buffer_write(&rb, data, sizeof(data));

    uint8_t output[10];
    size_t read = ring_buffer_read(&rb, output, sizeof(output));

    // Should only read what's available
    EXPECT_EQ(read, sizeof(data));
    EXPECT_TRUE(ring_buffer_is_empty(&rb));
}

// Parameterized test for different buffer sizes
class RingBufferSizeTest : public ::testing::TestWithParam<size_t> {
protected:
    uint8_t *backing;
    ring_buffer_t rb;

    void SetUp() override {
        size_t size = GetParam();
        backing = new uint8_t[size];
        ring_buffer_init(&rb, backing, size);
    }

    void TearDown() override {
        delete[] backing;
    }
};

TEST_P(RingBufferSizeTest, BasicOperations) {
    size_t capacity = GetParam();
    size_t test_size = capacity / 2;

    uint8_t *data = new uint8_t[test_size];
    for (size_t i = 0; i < test_size; i++) {
        data[i] = static_cast<uint8_t>(i);
    }

    size_t written = ring_buffer_write(&rb, data, test_size);
    EXPECT_EQ(written, test_size);

    uint8_t *output = new uint8_t[test_size];
    size_t read = ring_buffer_read(&rb, output, test_size);

    EXPECT_EQ(read, test_size);
    EXPECT_EQ(memcmp(data, output, test_size), 0);

    delete[] data;
    delete[] output;
}

INSTANTIATE_TEST_SUITE_P(
    DifferentSizes,
    RingBufferSizeTest,
    ::testing::Values(8, 16, 32, 64, 128, 256, 1024)
);
