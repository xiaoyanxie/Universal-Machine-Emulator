# Universal Machine Emulator (Imported Folder)

This directory is a reorganized UM project snapshot. It keeps source files at the top level and moves generated/auxiliary artifacts into focused folders.

## Layout
- `bitpack.c`, `driver.c`, `instructions.c`, `memory.c`, `test.c`: Core source files.
- `bitpack.h`, `instructions.h`, `memory.h`, `um-dis.h`, `um-opcode.h`: Headers.
- `Makefile`: Build script for this snapshot.
- `docs/`: Design, notes, and performance analysis.
- `tests/`: Test inventory and generated test artifacts.
- `outputs/`: Collected output files.
- `artifacts/`: Built binaries and static libraries.
- `data/`: Additional binary/input data files.
- `benchmarks/`: Benchmark UM binaries and stdin input.
- `profiling/`: Profiling benchmark bundle and profiling notes.
- `submission/`: Original submission snapshot preserved as-is.

## Build
```bash
make
```

If your environment has COMP 40 libraries configured in the include/lib paths from `Makefile`, this builds the emulator executable.

## Run
```bash
./artifacts/bin/um benchmarks/umbin/hello.um
./artifacts/bin/um benchmarks/umbin/midmark.um
./artifacts/bin/um benchmarks/umbin/sandmark.umz
./artifacts/bin/um benchmarks/umbin/advent.umz < benchmarks/advent.in
```

## Notes
- The original class-style README has been moved to `docs/performance-analysis.md`.
- Generated test files are in `tests/generated/`.
- Existing built outputs were moved to `artifacts/` for cleanliness.

## License
This snapshot includes an MIT `LICENSE` file.
