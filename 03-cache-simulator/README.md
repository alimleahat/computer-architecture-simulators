# Two-Level Cache Simulator

A C++ simulator that runs a memory access trace through an L1 and L2 cache and reports what each access does at every level.

L1 and L2 are exclusive: a block lives in one cache or the other, and blocks evicted from L1 move down to L2. Both caches are write-back and write no-allocate. Empty ways fill lowest-numbered first, and full sets evict with a per-set round-robin counter. Block size, associativity (0 for fully associative), and size in KiB are read from the config file.

## Run

From this folder:

```sh
make
./cachesimulator.out cacheconfig.txt trace.txt
```

- `cacheconfig.txt` sets the block size, associativity, and size of each cache.
- `trace.txt` lists the reads (`R`) and writes (`W`) with 32-bit hex addresses.
- `trace.txt.out` records three codes per access: the L1 event, the L2 event, and whether main memory was written (0 no access, 1 read hit, 2 read miss, 3 write hit, 4 write miss, 5 no memory write, 6 memory write).
- `expected_results/` contains the reference output for the sample.

The sample's output matches the reference line for line. The sample config is direct-mapped, so round-robin eviction was checked separately with a small hand-worked trace. This checks the bundled sample, not every possible configuration.
