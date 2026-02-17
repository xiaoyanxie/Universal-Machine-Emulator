# UM Tests

This directory tracks UM-related test artifacts and test inventory.

## Files
- `tests/UMTESTS.txt`: Names of expected UM test programs.
- `umlab.c`: UM test program builders.
- `umlabwrite.c`: Emits `.um` test binaries and optional `.0`/`.1` I/O files.

## Generating Test Binaries
If your environment has the COMP 40 libraries configured in the `Makefile` paths:
```bash
make writetests
./writetests
```

This generates files like:
- `test_halt.um`
- `test_addition.um`
- `test_input.0` and `test_input.1`

Place generated files under:
- `tests/generated/`
