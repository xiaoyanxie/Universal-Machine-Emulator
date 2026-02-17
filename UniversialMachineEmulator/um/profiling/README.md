# Profiling Note

Profiling assets are maintained at the repository root:
- `profiling/`

This local folder is kept only as a pointer to avoid duplicated profiling data
inside `UniversialMachineEmulator/um/`.

To run a benchmark in this imported snapshot:
```bash
./artifacts/bin/um benchmarks/umbin/midmark.um > /dev/null
```
