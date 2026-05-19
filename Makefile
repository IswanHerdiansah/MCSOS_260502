.RECIPEPREFIX := >
SHELL := /usr/bin/env bash

ARCH := x86_64
BUILD_DIR := build
KERNEL := $(BUILD_DIR)/kernel.elf
MAP := $(BUILD_DIR)/kernel.map

CC := clang
LD := ld.lld

CFLAGS := --target=x86_64-unknown-none-elf -std=c17 -ffreestanding \
-fno-stack-protector -fno-stack-check -fno-pic -fno-pie -fno-lto \
-m64 -march=x86-64 -mabi=sysv -mno-red-zone \
-Ikernel -Ikernel/arch/x86_64/include

LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -T linker.ld -Map=$(MAP)

SRC_C := $(shell find kernel -name '*.c' | LC_ALL=C sort)
OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC_C))

.PHONY: all build inspect image run debug check-src check-scripts grade clean distclean

all: build

check-src:
>$(CC) --version | head -n 1
>$(LD) --version | head -n 1
>test -f linker.ld
>test -d kernel/core
>test -d kernel/lib
>test -d kernel/arch/x86_64/include

check-scripts:
>for s in tools/scripts/*.sh; do bash -n "$$s"; done

build: $(KERNEL)

$(BUILD_DIR)/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -o $@ $(OBJ)

inspect: $(KERNEL)
>./tools/scripts/inspect_kernel.sh

image: $(KERNEL)
>./tools/scripts/make_iso.sh

run: image
>bash tools/scripts/run_qemu.sh

debug: image
>bash tools/scripts/run_qemu_debug.sh

grade: check-src check-scripts build inspect image
>bash tools/scripts/run_qemu.sh
>bash tools/scripts/grade_m2.sh

clean:
>rm -rf $(BUILD_DIR)/kernel $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.map $(BUILD_DIR)/inspect

distclean:
>rm -rf $(BUILD_DIR) iso_root

# =========================
# META (REQUIRED FOR M0/M1 PREFLIGHT)
# =========================

meta:
>mkdir -p build/meta
>echo "clang: $(shell clang --version | head -n 1)" > build/meta/toolchain-versions.txt
>echo "ld.lld: $(shell ld.lld --version | head -n 1)" >> build/meta/toolchain-versions.txt
>echo "qemu: $(shell qemu-system-x86_64 --version | head -n 1)" >> build/meta/toolchain-versions.txt
>echo "xorriso: $(shell xorriso --version | head -n 1)" >> build/meta/toolchain-versions.txt
>echo "make: $(shell make --version | head -n 1)" >> build/meta/toolchain-versions.txt

.PHONY: meta
