#!/usr/bin/env bash
set -euo pipefail

KERNEL="build/kernel.elf"
OUTDIR="build/inspect"

mkdir -p "$OUTDIR"

readelf -h "$KERNEL" | tee "$OUTDIR/readelf-header.txt"
readelf -l "$KERNEL" | tee "$OUTDIR/readelf-program-headers.txt"
objdump -drwC "$KERNEL" > "$OUTDIR/objdump-disassembly.txt"
nm -n "$KERNEL" > "$OUTDIR/nm-symbols.txt"

grep -q 'Class:.*ELF64' "$OUTDIR/readelf-header.txt"
grep -q 'Machine:.*Advanced Micro Devices X86-64' "$OUTDIR/readelf-header.txt"

grep -Eq 'Entry point address:.*0xffffffff8000[0-9a-f]+' \
    "$OUTDIR/readelf-header.txt"

echo "OK: kernel ELF inspection passed"
