Benchmarks:
```shell
Run on (8 X 4700 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 1280 KiB (x4)
  L3 Unified 12288 KiB (x1)
Load Average: 4.24, 2.44, 1.69
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
simple_spinlock/iterations:10             176 ms        0.233 ms           10
readable_spinlock/iterations:10          26.2 ms        0.179 ms           10
mm_readable_spinlock/iterations:10       12.2 ms        0.134 ms           10
queue_spinlock/iterations:10              181 ms        0.160 ms           10
```

