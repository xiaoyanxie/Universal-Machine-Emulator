# Universal Machine Emulator 🤖

[![Language C](https://img.shields.io/badge/language-C-00599C?logo=c&logoColor=white)](#)
[![License MIT](https://img.shields.io/badge/license-MIT-2ea44f)](LICENSE)
[![Project Type](https://img.shields.io/badge/type-UM%20emulator-ff69b4)](#)

A C implementation of the COMP 40 Universal Machine (UM) with benchmarks, profiling artifacts, and unit-test generators.

## ✨ What's Inside
- UM emulator implementation in C (`driver.c`, `instructions.c`, `memory.c`, `bitpack.c`)
- Official UM benchmark/program binaries in `umbin/`
- Profiling workflow and historical outputs in `profiling/`
- UM test inventory and guidance in `tests/`

## 🗂️ Repository Layout
- `driver.c`, `instructions.c`, `memory.c`, `bitpack.c`: Core emulator
- `instructions.h`, `memory.h`, `bitpack.h`: Public interfaces
- `umbin/`: UM binaries (`.um`/`.umz`) and benchmark assets
- `profiling/`: Callgrind outputs, charts, and profiling notes
- `tests/`: UM test inventory and test-generation notes
- `docs/`: Performance analysis notes and UM file catalog

## 🛠️ Build
Prerequisites:
- `gcc`
- COMP 40 support libraries (`cii40`, `bitpack`, `40locality`) in the paths used by `Makefile`

Build emulator:
```bash
make
```

Optional test generator:
```bash
make writetests
```

## ▶️ Run
```bash
./um umbin/hello.um
./um umbin/midmark.um
./um umbin/sandmark.umz
./um umbin/advent.umz < advent
```

## 📊 Profiling
```bash
make profile-midmark
make profile-sandmark
make profile-advent
```

Or:
```bash
./profiling/run_callgrind.sh umbin/midmark.um midmark
./profiling/run_callgrind.sh umbin/advent.umz advent advent
```

More details: `profiling/README.md`

## 🧪 UM Test Files
- Test name inventory: `tests/UMTESTS.txt`
- Generated test artifact directory: `tests/generated/`
- Test generator sources: `umlab.c`, `umlabwrite.c`
- Full file catalog: `docs/UM_FILES.md`

## 📝 Performance Analysis
- `docs/performance-analysis.md`

## 📄 License
MIT License in `LICENSE`.
