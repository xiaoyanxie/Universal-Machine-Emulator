#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
    echo "Usage: $0 <program.um|program.umz> <output-name> [stdin-file]" >&2
    exit 1
fi

program="$1"
output_name="$2"
stdin_file="${3:-}"
out_file="profiling/callgrind/${output_name}.out"

mkdir -p profiling/callgrind

if [[ -n "$stdin_file" ]]; then
    valgrind --tool=callgrind --dump-instr=yes \
        --callgrind-out-file="$out_file" \
        ./um "$program" < "$stdin_file" > /dev/null
else
    valgrind --tool=callgrind --dump-instr=yes \
        --callgrind-out-file="$out_file" \
        ./um "$program" > /dev/null
fi

echo "Wrote $out_file"
