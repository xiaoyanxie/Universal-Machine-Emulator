# Universal Machine Emulator (Imported Snapshot) 🧸

[![Language C](https://img.shields.io/badge/language-C-00599C?logo=c&logoColor=white)](#)
[![Usage](https://img.shields.io/badge/usage-reference%20only-orange)](#)
[![Snapshot](https://img.shields.io/badge/folder-imported%20snapshot-f4a460)](#)

This directory is a reorganized UM project snapshot. Source stays near the top-level, and generated artifacts are grouped into focused folders.

## ⚠️ Academic Integrity Notice
This repository is for portfolio and learning reference only.
Do not copy, submit, or adapt this code for coursework or academic evaluation.

## 🗂️ Layout
- `bitpack.c`, `driver.c`, `instructions.c`, `memory.c`, `test.c`: Core source files
- `bitpack.h`, `instructions.h`, `memory.h`, `um-dis.h`, `um-opcode.h`: Headers
- `Makefile`: Build script for this snapshot
- `docs/`: Design, notes, and performance analysis
- `tests/`: Test inventory and generated test artifacts
- `outputs/`: Collected output files
- `artifacts/`: Built binaries and static libraries
- `data/`: Additional binary/input data files
- `benchmarks/`: Benchmark UM binaries and stdin input
- `profiling/`: Pointer note to canonical root-level profiling assets
- `submission/`: Original submission snapshot preserved as-is

## 🛠️ Build
```bash
make
```

If your environment has COMP 40 libraries configured in the include/lib paths from `Makefile`, this builds `artifacts/bin/um`.

## ▶️ Run
```bash
./artifacts/bin/um benchmarks/umbin/hello.um
./artifacts/bin/um benchmarks/umbin/midmark.um
./artifacts/bin/um benchmarks/umbin/sandmark.umz
./artifacts/bin/um benchmarks/umbin/advent.umz < benchmarks/advent.in
```

## 📝 Notes
- Original class README: `docs/performance-analysis.md`
- Generated test files: `tests/generated/`
- Canonical profiling artifacts: repository root `profiling/`

## 📄 Usage Terms
No open-source license is granted for reuse in coursework submissions.
All rights reserved unless explicit permission is provided by the author.
