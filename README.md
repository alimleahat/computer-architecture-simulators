# Computer Architecture Simulators

Small C++ projects exploring instruction execution, processor pipelines, and memory systems.

## Projects

- [x] [Single-cycle MIPS](01-single-cycle-mips/) - instruction decoding, arithmetic, memory access, branches, and jumps.
- [x] [Pipelined MIPS](02-pipelined-mips/) - five-stage execution with forwarding, load-use stalls, and branch handling.
- [x] [Two-level cache simulator](03-cache-simulator/) - exclusive L1/L2 caches with write-back, write no-allocate, and round-robin eviction.
- [ ] Virtual memory simulator
- [ ] Branch prediction simulator
- [ ] Out-of-order execution simulator

## Run

Requires a C++ compiler available as `g++` and `make`.

```sh
git clone https://github.com/alimleahat/computer-architecture-simulators.git
cd computer-architecture-simulators/01-single-cycle-mips
make
./MIPS
```

For the pipeline simulator, run `make` and `./MIPS_pipeline` from `02-pipelined-mips/`.

For the cache simulator, run `make` and `./cachesimulator.out cacheconfig.txt trace.txt` from `03-cache-simulator/`.

Each project includes sample input files, reference outputs, and a short README. Remove generated results before repeating a run because register and pipeline traces append to existing files.

The first three projects extend starter code from NYU ECE-GY 6913.
