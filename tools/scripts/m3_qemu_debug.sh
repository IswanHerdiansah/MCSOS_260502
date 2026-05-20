#!/usr/bin/env bash
set -Eeuo pipefail

ISO="${1:-build/mcsos.iso}"
OVMF_CODE="${OVMF_CODE:-/usr/share/OVMF/OVMF_CODE.fd}"
OVMF_VARS_TEMPLATE="${OVMF_VARS_TEMPLATE:-ovmf/OVMF_VARS.fd}"
OVMF_VARS_RUN="/tmp/mcsos_ovmf_vars_debug_$$.fd"

test -f "$ISO"                || { echo "FAIL: ISO tidak ditemukan: $ISO" >&2; exit 1; }
test -f "$OVMF_VARS_TEMPLATE" || { echo "FAIL: OVMF_VARS template tidak ditemukan" >&2; exit 1; }

cp "$OVMF_VARS_TEMPLATE" "$OVMF_VARS_RUN"
trap 'rm -f "$OVMF_VARS_RUN"' EXIT

echo "[m3_qemu_debug] QEMU menunggu GDB di port 1234 ..."
exec qemu-system-x86_64 \
    -machine q35 \
    -m 256M \
    -smp 1 \
    -cpu qemu64 \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,file="$OVMF_VARS_RUN" \
    -cdrom "$ISO" \
    -boot order=d,once=d \
    -serial mon:stdio \
    -display none \
    -no-reboot \
    -no-shutdown \
    -s -S

