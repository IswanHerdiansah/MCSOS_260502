#!/usr/bin/env bash

set -eu

mkdir -p logs/m10

qemu-system-x86_64 \
    -machine q35 \
    -m 256M \
    -serial file:logs/m10/qemu_serial.log \
    -display none \
    -no-reboot \
    -no-shutdown \
    -cdrom build/mcsos.iso
