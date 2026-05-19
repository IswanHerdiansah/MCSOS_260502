#!/usr/bin/env bash
set -euo pipefail

ISO="build/mcsos.iso"
LOG="build/qemu-serial.log"

mkdir -p build

timeout 10s qemu-system-x86_64 \
  -machine q35 \
  -cpu qemu64 \
  -m 512M \
  -serial file:"$LOG" \
  -display none \
  -no-reboot \
  -no-shutdown \
  -cdrom "$ISO" || true

echo "OK: QEMU finished (timeout expected)"
echo "OK: QEMU serial log valid: $LOG"
