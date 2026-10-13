# Single-Cycle MIPS Simulator

A C++ simulator that fetches, decodes, and executes one MIPS instruction at a time.

Supports `addu`, `subu`, `and`, `or`, `nor`, `addiu`, `lw`, `sw`, `beq`, `j`, and a custom `halt` instruction. Instruction and data memory use big-endian words stored as one binary byte per line.

## Run

From this folder:

```sh
make
rm -f RFresult.txt dmemresult.txt
./MIPS
```

- `imem.txt` and `dmem.txt` provide the sample program and initial data.
- `RFresult.txt` records the register file after each executed instruction.
- `dmemresult.txt` records the final data memory.
- `expected_results/` contains the reference outputs for the sample.

The sample's register and memory results were checked against the references, ignoring Windows/Unix line-ending differences. This checks the bundled sample, not every possible program.
