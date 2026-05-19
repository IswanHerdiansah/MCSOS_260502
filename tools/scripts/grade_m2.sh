#!/usr/bin/env bash
set -euo pipefail

required_files=(
  build/kernel.elf
  build/kernel.map
  build/inspect/readelf-header.txt
  build/inspect/readelf-program-headers.txt
  build/inspect/objdump-disassembly.txt
  build/inspect/nm-symbols.txt
  build/mcsos.iso
  build/mcsos.iso.sha256
  build/qemu-serial.log
)

for f in "${required_files[@]}"; do
  test -s "$f"
  echo "OK artifact: $f"
done

grep -q 'ELF64' build/inspect/readelf-header.txt
grep -q 'Advanced Micro Devices X86-64' build/inspect/readelf-header.txt
grep -q '0xffffffff80000000' build/inspect/readelf-header.txt

grep -q 'MCSOS 260502 M2 boot path entered' build/qemu-serial.log
grep -q '\[M2\] early serial online' build/qemu-serial.log
grep -q '\[M2\] kernel reached controlled halt loop' build/qemu-serial.log

echo "OK: M2 local grading checks passed"
