# Profiling Guide

This directory keeps profiling-related files organized in one place.

## Structure
- `profiling/callgrind/`: Raw Callgrind output files.
- `profiling/visuals/`: Generated charts/images.
- `profiling/reports/`: PDF/XLSX profiling reports.
- `profiling/outputs/`: Text outputs captured during profiling runs.
- `profiling/commands.txt`: Canonical command list.

## Quick Start
Run one benchmark profile:
```bash
./profiling/run_callgrind.sh umbin/midmark.um midmark
```

Run with stdin input file:
```bash
./profiling/run_callgrind.sh umbin/advent.umz advent advent
```

Outputs are written to:
- `profiling/callgrind/<name>.out`

## Makefile Targets
Equivalent shortcuts are available:
```bash
make profile-midmark
make profile-sandmark
make profile-advent
```

## Visualizing
Open an output file in qcachegrind:
```bash
qcachegrind profiling/callgrind/midmark.out
```
