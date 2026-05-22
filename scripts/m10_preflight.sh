#!/usr/bin/env bash

set -eu

echo "======================================"
echo "[M10] preflight checks"
echo "======================================"

test -f kernel/include/mcsos/kernel/syscall.h
test -f kernel/core/syscall.c
test -f kernel/arch/x86_64/syscall_entry.S
test -f tests/test_syscall.c

echo "[M10] required files exist"

grep -q "mcsos_syscall_dispatch" \
    kernel/core/syscall.c

grep -q "x86_64_syscall_int80_stub" \
    kernel/arch/x86_64/syscall_entry.S

echo "[M10] symbols verified"

echo "======================================"
echo "[M10] preflight PASS"
echo "======================================"
