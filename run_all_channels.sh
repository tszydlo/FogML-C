#!/usr/bin/env bash
#
# Runs every per-channel dataset CSV in dataset/ through the compiled fogml_c
# binary, one file at a time.

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY="$SCRIPT_DIR/bin/fogml_c"
DATASET_DIR="$SCRIPT_DIR/dataset"

if [ ! -x "$BINARY" ]; then
    echo "Error: $BINARY not found or not executable. Build it first with 'make'." >&2
    exit 1
fi

shopt -s nullglob
csv_files=("$DATASET_DIR"/CADC*.csv)
shopt -u nullglob

if [ ${#csv_files[@]} -eq 0 ]; then
    echo "Error: no per-channel CSV files found in $DATASET_DIR" >&2
    exit 1
fi

for csv in "${csv_files[@]}"; do
    echo "=== $(basename "$csv") ==="
    "$BINARY" "$csv"
    echo
done
