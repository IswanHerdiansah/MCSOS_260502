#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

clang \
  -std=c17 \
  -Wall -Wextra -Werror \
  -ffreestanding \
  -fno-builtin \
  -fno-stack-protector \
  -mno-red-zone \
  -Ikernel/include \
  -c kernel/core/vmm.c \
  -o build/vmm.o

clang \
  -std=c17 \
  -Wall -Wextra -Werror \
  -Ikernel/include \
  kernel/core/vmm.c \
  tests/test_vmm_host.c \
  -o build/test_vmm_host

./build/test_vmm_host

nm -u build/vmm.o

echo "[M7] static grade PASS"
