# Unit tests for data_structures library

include(TestFramework)

# Host-based test using Google Test
add_robotics_gtest(
    NAME test_ring_buffer_gtest
    SOURCES test_ring_buffer_gtest.cpp
    DEPENDENCIES robotics::data_structures
)

# Target test using simple assertions
add_robotics_target_test(
    NAME test_ring_buffer_target
    SOURCES test_ring_buffer_target.c
    DEPENDENCIES robotics::data_structures
)
