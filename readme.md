[![CMake](https://github.com/k-morozov/sync_cpp/actions/workflows/cmake.yaml/badge.svg)](https://github.com/k-morozov/sync_cpp/actions/workflows/cmake.yaml)
## sync cpp library

#### Required:
- g++-12
- Conan 1.59.0

### atomic counter
-  benchmark

| Benchmark                    | Time     | CPU    | Iterations |
|------------------------------|----------|--------|------------|
| simple_counter/iterations:10 | 21221 us | 352 us | 10         |
| shards_counter/iterations:10 | 4953 us  | 247 us | 10         |
|

### Channels:
- buffered channels.
- unbuffered channels.
- buffered channels with MPMC queue.