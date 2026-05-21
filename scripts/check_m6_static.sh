#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

clang -std=c17 -Wall -Wextra -Werror \
  -ffreestanding -fno-builtin \
  -fno-stack-protector -mno-red-zone \
  -Ikernel/include \
  -c kernel/core/pmm.c \
  -o build/pmm.o

clang -std=c17 -Wall -Wextra -Werror \
  -Ikernel/include \
  kernel/core/pmm.c \
  tests/test_pmm_host.c \
  -o build/test_pmm_host

./build/test_pmm_host

nm -u build/pmm.o

echo "[PASS] M6 static check selesai"
