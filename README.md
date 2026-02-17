# Universal Machine Emulator

A C implementation of the COMP 40 Universal Machine (UM) with benchmark programs, profiling artifacts, and unit-test generators.

## What Is Included
- UM emulator implementation in C (`driver.c`, `instructions.c`, `memory.c`, `bitpack.c`).
- Official UM benchmark/program binaries in `umbin/`.
- Profiling workflow and historical profiling outputs in `profiling/`.
- UM test inventory and guidance in `tests/`.

## Repository Layout
- `driver.c`, `instructions.c`, `memory.c`, `bitpack.c`: Core emulator implementation.
- `instructions.h`, `memory.h`, `bitpack.h`: Public interfaces.
- `umbin/`: UM binaries (`.um`/`.umz`) and benchmark assets.
- `profiling/`: Callgrind outputs, charts, and profiling notes.
- `tests/`: UM test inventory and test-generation notes.
- `docs/`: Performance analysis notes and UM file catalog.

## Build
Prerequisites:
- `gcc`
- COMP 40 support libraries (`cii40`, `bitpack`, `40locality`) available in the include/lib paths used by the `Makefile`

Build the emulator:
```bash
make
```

Optional test generator build:
```bash
make writetests
```

## Run
Examples:
```bash
./um umbin/hello.um
./um umbin/midmark.um
./um umbin/sandmark.umz
./um umbin/advent.umz < advent
```

## Profiling
Use one of the following:
```bash
make profile-midmark
make profile-sandmark
make profile-advent
```

Or run the helper directly:
```bash
./profiling/run_callgrind.sh umbin/midmark.um midmark
./profiling/run_callgrind.sh umbin/advent.umz advent advent
```

Detailed notes are in `profiling/README.md`.

## UM-Related Files
- UM binaries and benchmarks: `umbin/`
- UM test name inventory: `tests/UMTESTS.txt`
- Generated UM test artifact directory: `tests/generated/`
- Test generator sources: `umlab.c`, `umlabwrite.c`
- Catalog of UM files: `docs/UM_FILES.md`

## Performance Analysis
The original class report-style README was preserved at:
- `docs/performance-analysis.md`

## License
This project is licensed under the MIT License. See `LICENSE`.
