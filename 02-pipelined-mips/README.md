# Five-Stage MIPS Pipeline Simulator

A cycle-by-cycle C++ simulator with fetch, decode, execute, memory, and writeback stages.

Supports `addu`, `subu`, `lw`, `sw`, `bne`, and a custom `halt` instruction. MEM/WB forwarding supplies newer operands to EX; load-use stalls hold fetch and decode while inserting an EX bubble. Branches resolve in decode, and HALT stops fetching while older instructions finish.

The simulator assumes branches have no RAW dependency on their preceding two instructions.

## Run

From this folder:

```sh
make
rm -f RFresult.txt stateresult.txt dmemresult.txt
./MIPS_pipeline
```

- `imem.txt` and `dmem.txt` provide the sample program and initial data.
- `RFresult.txt` records the register file each cycle.
- `stateresult.txt` records the pipeline registers each cycle.
- `dmemresult.txt` records the final data memory.
- `expected_results/` contains the reference outputs for the sample.

The sample's register and memory outputs match the references. The state trace differs in unused fields and includes four extra EX control fields, so it is not a byte-for-byte reference match.
