# data_structures

Cross-platform data structures for embedded and robotics systems.

## Features

- **Lock-free ring buffer**: Single-producer single-consumer FIFO for real-time systems
- **No dynamic allocation**: All data structures support user-provided backing storage
- **C compatible**: Pure C API for maximum portability

## Components

### ring_buffer

A lock-free FIFO buffer for passing data between threads or interrupt contexts.

```c
#include "data_structures/ring_buffer.h"

uint8_t backing[256];
ring_buffer_t rb;

ring_buffer_init(&rb, backing, sizeof(backing));

uint8_t data[] = {1, 2, 3, 4};
ring_buffer_write(&rb, data, sizeof(data));

uint8_t output[4];
ring_buffer_read(&rb, output, sizeof(output));
```

## Building

```bash
# Build as part of monorepo
cmake -S . -B build
cmake --build build

# Build standalone
cmake -S libs/data_structures -B build/data_structures
cmake --build build/data_structures

# Run tests
ctest --test-dir build/data_structures
```
