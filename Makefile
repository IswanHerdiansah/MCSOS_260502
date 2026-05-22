.RECIPEPREFIX := >
SHELL := /usr/bin/env bash

# ── Direktori ───────────────────────────────────────────────────────────────
BUILD_DIR  := build
ISO_ROOT   := iso_root
LIMINE_DIR := limine

# ── Artefak utama ───────────────────────────────────────────────────────────
KERNEL        := $(BUILD_DIR)/kernel.elf
BP_KERNEL     := $(BUILD_DIR)/kernel.breakpoint.elf
PANIC_KERNEL  := $(BUILD_DIR)/kernel.panic.elf

MAP           := $(BUILD_DIR)/kernel.map
BP_MAP        := $(BUILD_DIR)/kernel.breakpoint.map
PANIC_MAP     := $(BUILD_DIR)/kernel.panic.map

DISASM        := $(BUILD_DIR)/kernel.disasm.txt
SYMS          := $(BUILD_DIR)/kernel.syms.txt

ISO           := $(BUILD_DIR)/mcsos.iso
PANIC_ISO     := $(BUILD_DIR)/mcsos.panic.iso
BP_ISO        := $(BUILD_DIR)/mcsos.breakpoint.iso

# ── Toolchain ───────────────────────────────────────────────────────────────
CC      := clang
LD      := ld.lld
OBJDUMP := objdump
READELF := readelf
NM      := nm

# ── Flags kompilasi ─────────────────────────────────────────────────────────
COMMON_CFLAGS := \
    --target=x86_64-unknown-none-elf -std=c17 \
    -ffreestanding -fno-builtin -fno-stack-protector -fno-stack-check \
    -fno-pic -fno-pie -fno-lto \
    -m64 -march=x86-64 -mabi=sysv -mno-red-zone \
    -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel \
    -Wall -Wextra -Werror \
    -Ikernel/arch/x86_64/include -Ikernel/include

COMMON_ASFLAGS := \
    --target=x86_64-unknown-none-elf \
    -ffreestanding -fno-pic -fno-pie \
    -m64 -mno-red-zone \
    -Wall -Wextra -Werror \
    -Ikernel/arch/x86_64/include -Ikernel/include

CFLAGS        := $(COMMON_CFLAGS)
ASFLAGS       := $(COMMON_ASFLAGS)

BP_CFLAGS     := $(COMMON_CFLAGS) -DMCSOS_M4_TRIGGER_BREAKPOINT=1
PANIC_CFLAGS  := $(COMMON_CFLAGS) -DMCSOS_M3_TRIGGER_PANIC=1 -DMCSOS_M4_TRIGGER_PANIC=1

LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -T linker.ld

# ── Sumber ──────────────────────────────────────────────────────────────────
SRC_C := $(shell find kernel -name '*.c' | LC_ALL=C sort)
SRC_S := $(shell find kernel -name '*.S' | LC_ALL=C sort)

OBJ := \
    $(patsubst %.c,$(BUILD_DIR)/normal/%.o,$(SRC_C)) \
    $(patsubst %.S,$(BUILD_DIR)/normal/%.o,$(SRC_S))

BP_OBJ := \
    $(patsubst %.c,$(BUILD_DIR)/breakpoint/%.o,$(SRC_C)) \
    $(patsubst %.S,$(BUILD_DIR)/breakpoint/%.o,$(SRC_S))

PANIC_OBJ := \
    $(patsubst %.c,$(BUILD_DIR)/panic/%.o,$(SRC_C)) \
    $(patsubst %.S,$(BUILD_DIR)/panic/%.o,$(SRC_S))

# ── Phony targets ───────────────────────────────────────────────────────────
.PHONY: all build breakpoint panic inspect audit \
image image-panic image-breakpoint \
iso iso-panic iso-breakpoint \
clean distclean meta
# ── Default ─────────────────────────────────────────────────────────────────
all: build inspect

# ════════════════════════════════════════════════════════════════════════════
# BUILD
# ════════════════════════════════════════════════════════════════════════════
build: $(KERNEL)

breakpoint: $(BP_KERNEL)

panic: $(PANIC_KERNEL)

$(BUILD_DIR)/normal/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/normal/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/breakpoint/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(BP_CFLAGS) -c $< -o $@

$(BUILD_DIR)/breakpoint/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/panic/%.o: %.c
>mkdir -p $(dir $@)
>$(CC) $(PANIC_CFLAGS) -c $< -o $@

$(BUILD_DIR)/panic/%.o: %.S
>mkdir -p $(dir $@)
>$(CC) $(ASFLAGS) -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -Map=$(MAP) -o $@ $(OBJ)

$(BP_KERNEL): $(BP_OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -Map=$(BP_MAP) -o $@ $(BP_OBJ)

$(PANIC_KERNEL): $(PANIC_OBJ) linker.ld
>mkdir -p $(BUILD_DIR)
>$(LD) $(LDFLAGS) -Map=$(PANIC_MAP) -o $@ $(PANIC_OBJ)

# ════════════════════════════════════════════════════════════════════════════
# INSPECT
# ════════════════════════════════════════════════════════════════════════════
inspect: $(KERNEL)
>$(READELF) -h $(KERNEL) > $(BUILD_DIR)/kernel.readelf.header.txt
>$(READELF) -l $(KERNEL) > $(BUILD_DIR)/kernel.readelf.programs.txt
>$(NM) -n $(KERNEL) > $(SYMS)
>$(OBJDUMP) -d -Mintel $(KERNEL) > $(DISASM)
>grep -q 'ELF64' $(BUILD_DIR)/kernel.readelf.header.txt
>grep -q 'Machine:[[:space:]]*Advanced Micro Devices X86-64' $(BUILD_DIR)/kernel.readelf.header.txt
>grep -q 'kmain' $(SYMS)
>grep -q 'kernel_panic_at' $(SYMS)
>grep -q 'cpu_halt_forever' $(DISASM)
>grep -q 'x86_64_idt_init' $(SYMS)
>grep -q 'x86_64_trap_dispatch' $(SYMS)
>grep -q 'iretq' $(DISASM)
>grep -q 'lidt' $(DISASM)

# ════════════════════════════════════════════════════════════════════════════
# AUDIT
# ════════════════════════════════════════════════════════════════════════════
audit: inspect breakpoint panic
>! $(NM) -u $(KERNEL) | grep .
>! $(NM) -u $(BP_KERNEL) | grep .
>! $(NM) -u $(PANIC_KERNEL) | grep .
>grep -q 'kernel_panic_at' $(DISASM)
>grep -q 'isr_stub_14' $(SYMS)
>grep -q 'x86_64_exception_stubs' $(SYMS)
>$(READELF) -S $(KERNEL) | grep -q '.text'
>$(READELF) -S $(KERNEL) | grep -q '.rodata'

# ════════════════════════════════════════════════════════════════════════════
# IMAGE — helper internal untuk isi iso_root
# ════════════════════════════════════════════════════════════════════════════
define build_iso
        @test -f $(LIMINE_DIR)/limine-bios.sys || \
            { echo "ERROR: jalankan: make -C limine"; exit 1; }
        rm -rf $(ISO_ROOT)
        mkdir -p $(ISO_ROOT)/boot/limine $(ISO_ROOT)/EFI/BOOT
        cp $(1)                              $(ISO_ROOT)/boot/kernel.elf
        cp $(LIMINE_DIR)/limine-bios.sys     $(ISO_ROOT)/boot/limine/
        cp $(LIMINE_DIR)/limine-bios-cd.bin  $(ISO_ROOT)/boot/limine/
        cp $(LIMINE_DIR)/limine-uefi-cd.bin  $(ISO_ROOT)/boot/limine/
        cp $(LIMINE_DIR)/BOOTX64.EFI         $(ISO_ROOT)/EFI/BOOT/BOOTX64.EFI
        cp limine.conf                       $(ISO_ROOT)/boot/limine/limine.conf
        cp limine.conf                       $(ISO_ROOT)/EFI/BOOT/limine.conf
        xorriso -as mkisofs \
            -b boot/limine/limine-bios-cd.bin \
            -no-emul-boot -boot-load-size 4 -boot-info-table \
            --efi-boot boot/limine/limine-uefi-cd.bin \
            -efi-boot-part --efi-boot-image --protective-msdos-label \
            $(ISO_ROOT) -o $(2) 2>/dev/null
        $(LIMINE_DIR)/limine bios-install $(2) 2>/dev/null
        @echo "ISO selesai: $(2)"
endef

image: $(ISO)

iso: image

$(ISO): $(KERNEL) limine.conf
>$(call build_iso,$(KERNEL),$(ISO))

image-panic: $(PANIC_ISO)

iso-panic: image-panic

$(PANIC_ISO): $(PANIC_KERNEL) limine.conf
>$(call build_iso,$(PANIC_KERNEL),$(PANIC_ISO))

image-breakpoint: $(BP_ISO)

iso-breakpoint: image-breakpoint

$(BP_ISO): $(BP_KERNEL) limine.conf
>$(call build_iso,$(BP_KERNEL),$(BP_ISO))

# ════════════════════════════════════════════════════════════════════════════
# CLEAN
# ════════════════════════════════════════════════════════════════════════════
clean:
>rm -rf $(BUILD_DIR) $(ISO_ROOT)

distclean: clean
>rm -rf limine ovmf evidence

# ════════════════════════════════════════════════════════════════════════════
# META
# ════════════════════════════════════════════════════════════════════════════
meta:
>mkdir -p $(BUILD_DIR)/meta
>echo "clang: $$(clang --version | head -n 1)"              > $(BUILD_DIR)/meta/toolchain-versions.txt
>echo "ld.lld: $$(ld.lld --version | head -n 1)"           >> $(BUILD_DIR)/meta/toolchain-versions.txt
>echo "qemu: $$(qemu-system-x86_64 --version | head -n 1)" >> $(BUILD_DIR)/meta/toolchain-versions.txt
>echo "xorriso: $$(xorriso --version 2>&1 | head -n 1)"    >> $(BUILD_DIR)/meta/toolchain-versions.txt
>echo "make: $$(make --version | head -n 1)"               >> $(BUILD_DIR)/meta/toolchain-versions.txt

# ════════════════════════════════════════════════════════════════════════════
# M5 STATIC GRADE
# ════════════════════════════════════════════════════════════════════════════

GRADE_ELF := $(BUILD_DIR)/mcsos-m5.elf
GRADE_MAP := $(BUILD_DIR)/mcsos-m5.map

.PHONY: grade

grade: audit
>cp $(KERNEL) $(GRADE_ELF)
>cp $(MAP) $(GRADE_MAP)

>$(READELF) -h $(KERNEL) > $(BUILD_DIR)/readelf-header.txt
>$(READELF) -S $(KERNEL) > $(BUILD_DIR)/readelf-sections.txt
>$(READELF) -l $(KERNEL) > $(BUILD_DIR)/readelf-program-headers.txt

>$(NM) -n $(KERNEL) > $(BUILD_DIR)/symbols.txt
>$(NM) -u $(KERNEL) > $(BUILD_DIR)/undefined.txt

>$(OBJDUMP) -d -Mintel $(KERNEL) > $(BUILD_DIR)/disassembly.txt

>test ! -s $(BUILD_DIR)/undefined.txt

>grep -q 'lidt' $(BUILD_DIR)/disassembly.txt
>grep -q 'iretq' $(BUILD_DIR)/disassembly.txt
>grep -q 'sti' $(BUILD_DIR)/disassembly.txt
>grep -q 'hlt' $(BUILD_DIR)/disassembly.txt

>grep -q 'isr_stub_32' $(BUILD_DIR)/symbols.txt
>grep -q 'pic_remap' $(BUILD_DIR)/symbols.txt
>grep -q 'pit_configure_hz' $(BUILD_DIR)/symbols.txt
>grep -q 'timer_on_irq0' $(BUILD_DIR)/symbols.txt
>grep -q 'x86_64_trap_dispatch' $(BUILD_DIR)/symbols.txt

>@echo '[M5] static grade: PASS'

# ════════════════════════════════════════════════════════════════════════════
# M6 STATIC CHECK
# ════════════════════════════════════════════════════════════════════════════

HOSTCC := cc

M6_HOST_CFLAGS := \
    -std=c17 -Wall -Wextra -Werror \
    -Ikernel/include

.PHONY: check-m6

check-m6:
>mkdir -p build

>$(CC) \
    -std=c17 \
    -Wall -Wextra -Werror \
    -ffreestanding -fno-builtin \
    -fno-stack-protector \
    -mno-red-zone \
    -Ikernel/include \
    -c kernel/core/pmm.c \
    -o build/pmm.o

>$(HOSTCC) $(M6_HOST_CFLAGS) \
    kernel/core/pmm.c \
    tests/test_pmm_host.c \
    -o build/test_pmm_host

>./build/test_pmm_host

>$(NM) -u build/pmm.o | tee build/pmm.undefined.txt

>test ! -s build/pmm.undefined.txt

>$(OBJDUMP) -dr build/pmm.o > build/pmm.objdump.txt

>@echo '[M6] static grade: PASS'

# ════════════════════════════════════════════════════════════════════════════
# M7 TARGETS
# ════════════════════════════════════════════════════════════════════════════

.PHONY: check-m7
check-m7:
>./scripts/check_m7_static.sh

.PHONY: grade-m7
grade-m7:
>./scripts/grade_m7.sh

.PHONY: run-qemu-gdb
run-qemu-gdb: $(ISO)
>qemu-system-x86_64 \
    -cdrom $(ISO) \
    -serial stdio \
    -s -S

.PHONY: run-qemu-smoke
run-qemu-smoke: $(ISO)
>qemu-system-x86_64 \
    -cdrom $(ISO) \
    -serial stdio \
    -display none \
    -no-reboot \
    -no-shutdown

# ════════════════════════════════════════════════════════════════════════════
# M8 TARGETS
# ════════════════════════════════════════════════════════════════════════════

M8_BUILD_DIR := build/m8

.PHONY: m8-clean
m8-clean:
>rm -rf $(M8_BUILD_DIR)

$(M8_BUILD_DIR):
>mkdir -p $(M8_BUILD_DIR)

.PHONY: m8-kmem-freestanding
m8-kmem-freestanding: | $(M8_BUILD_DIR)
>$(CC) \
    -std=c17 \
    -Wall \
    -Wextra \
    -Werror \
    -ffreestanding \
    -fno-builtin \
    -fno-stack-protector \
    -mno-red-zone \
    -Ikernel/include \
    -c kernel/mm/kmem.c \
    -o $(M8_BUILD_DIR)/kmem.freestanding.o

.PHONY: m8-kmem-host-test
m8-kmem-host-test: | $(M8_BUILD_DIR)
>$(HOSTCC) \
    -std=c17 \
    -Wall \
    -Wextra \
    -Werror \
    -Ikernel/include \
    tests/test_kmem.c \
    kernel/mm/kmem.c \
    -o $(M8_BUILD_DIR)/test_kmem

>./$(M8_BUILD_DIR)/test_kmem | tee $(M8_BUILD_DIR)/test_kmem.log

.PHONY: m8-audit
m8-audit: m8-kmem-freestanding
>nm -u $(M8_BUILD_DIR)/kmem.freestanding.o | tee $(M8_BUILD_DIR)/nm_u.txt

>test ! -s $(M8_BUILD_DIR)/nm_u.txt

>readelf -h $(M8_BUILD_DIR)/kmem.freestanding.o > $(M8_BUILD_DIR)/readelf_h.txt

>objdump -dr $(M8_BUILD_DIR)/kmem.freestanding.o > $(M8_BUILD_DIR)/kmem.objdump.txt

.PHONY: check-m8
check-m8:
>./scripts/check_m8_kmem.sh

.PHONY: m8-all
m8-all: m8-kmem-host-test m8-audit

# ============================================================================
# M9 TARGETS
# ============================================================================

M9_BUILD_DIR := build/m9
M9_EVIDENCE_DIR := evidence/m9

.PHONY: m9-clean
m9-clean:
>rm -rf $(M9_BUILD_DIR)
>rm -rf $(M9_EVIDENCE_DIR)

$(M9_BUILD_DIR):
>mkdir -p $(M9_BUILD_DIR)

$(M9_EVIDENCE_DIR):
>mkdir -p $(M9_EVIDENCE_DIR)

.PHONY: m9-host-test
m9-host-test: | $(M9_BUILD_DIR) $(M9_EVIDENCE_DIR)

>$(HOSTCC) \
>   -std=c17 \
>   -Wall \
>   -Wextra \
>   -Werror \
>   -DMCSOS_HOST_TEST \
>   -Ikernel/include \
>   tests/test_scheduler.c \
>   kernel/core/thread.c \
>   kernel/mm/kmem.c \
>   -o build/m9/m9_host_test

>./build/m9/m9_host_test \
>   | tee build/m9/test_scheduler.log

.PHONY: m9-freestanding
m9-freestanding: | $(M9_BUILD_DIR)

>clang \
>   --target=x86_64-unknown-none-elf \
>   -std=c17 \
>   -ffreestanding \
>   -fno-builtin \
>   -fno-stack-protector \
>   -fno-pic \
>   -fno-pie \
>   -m64 \
>   -mno-red-zone \
>   -Wall \
>   -Wextra \
>   -Werror \
>   -DMCSOS_HOST_TEST \
>   -Ikernel/include \
>   -c kernel/core/thread.c \
>   -o build/m9/mcsos_thread.freestanding.o

>clang \
>   --target=x86_64-unknown-none-elf \
>   -ffreestanding \
>   -fno-stack-protector \
>   -fno-pic \
>   -fno-pie \
>   -m64 \
>   -mno-red-zone \
>   -Wall \
>   -Wextra \
>   -Werror \
>   -Ikernel/include \
>   -c kernel/arch/x86_64/context_switch.S \
>   -o build/m9/context_switch.o

>ld.lld -r \
>   build/m9/mcsos_thread.freestanding.o \
>   build/m9/context_switch.o \
>   -o build/m9/m9_scheduler_combined.o

>@echo "[M9] freestanding PASS"

.PHONY: m9-audit
m9-audit: m9-freestanding

>nm -u build/m9/m9_scheduler_combined.o \
>   | tee build/m9/nm_undefined.log

>test ! -s build/m9/nm_undefined.log

>readelf -h build/m9/m9_scheduler_combined.o \
>   | tee build/m9/readelf_header.log

>grep -q 'ELF64' build/m9/readelf_header.log
>grep -q 'Advanced Micro Devices X86-64' build/m9/readelf_header.log

>objdump -d build/m9/m9_scheduler_combined.o \
>   | grep -E 'mcsos_context_switch|jmp|ret|hlt' \
>   | tee build/m9/objdump_key.log

>grep -q 'mcsos_context_switch' build/m9/objdump_key.log
>grep -q 'hlt' build/m9/objdump_key.log

>sha256sum \
>   build/m9/m9_host_test \
>   build/m9/m9_scheduler_combined.o \
>   | tee build/m9/sha256.log

>@echo "[M9] audit PASS"

.PHONY: m9-all
m9-all: m9-host-test m9-audit

>@echo "======================================"
>@echo "[M9] scheduler milestone PASS"
>@echo "======================================"

# ============================================================================
# M10 TARGETS
# ============================================================================

M10_BUILD_DIR := build/m10

.PHONY: m10-clean
m10-clean:
>rm -rf build/m10
>rm -rf logs/m10

build/m10:
>mkdir -p build/m10

logs/m10:
>mkdir -p logs/m10

.PHONY: m10-host-test
m10-host-test: build/m10
>cc \
>   -std=c17 \
>   -Wall \
>   -Wextra \
>   -Werror \
>   -DMCSOS_HOST_TEST \
>   -Ikernel/include \
>   tests/test_syscall.c \
>   kernel/core/syscall.c \
>   -o build/m10/m10_host_test

>./build/m10/m10_host_test \
>   | tee build/m10/test_syscall.log

>@echo "[M10] host test PASS"

.PHONY: m10-freestanding
m10-freestanding: build/m10
>clang \
>   --target=x86_64-unknown-none-elf \
>   -std=c17 \
>   -ffreestanding \
>   -fno-builtin \
>   -fno-stack-protector \
>   -fno-pic \
>   -fno-pie \
>   -m64 \
>   -mno-red-zone \
>   -Wall \
>   -Wextra \
>   -Werror \
>   -Ikernel/include \
>   -c kernel/core/syscall.c \
>   -o build/m10/syscall.o

>clang \
>   --target=x86_64-unknown-none-elf \
>   -ffreestanding \
>   -fno-stack-protector \
>   -fno-pic \
>   -fno-pie \
>   -m64 \
>   -mno-red-zone \
>   -Wall \
>   -Wextra \
>   -Werror \
>   -Ikernel/include \
>   -c kernel/arch/x86_64/syscall_entry.S \
>   -o build/m10/syscall_entry.o

>ld.lld -r \
>   build/m10/syscall.o \
>   build/m10/syscall_entry.o \
>   -o build/m10/m10_syscall_combined.o

>@echo "[M10] freestanding PASS"

.PHONY: m10-audit
m10-audit: build/m10
>nm -u build/m10/m10_syscall_combined.o \
>   | tee build/m10/nm_undefined.log

>readelf -h build/m10/m10_syscall_combined.o \
>   | tee build/m10/readelf_header.log

>objdump -d build/m10/m10_syscall_combined.o \
>   | grep -E 'x86_64_syscall_int80_stub|iretq' \
>   | tee build/m10/objdump_key.log

>sha256sum \
>   build/m10/m10_host_test \
>   build/m10/m10_syscall_combined.o \
>   | tee build/m10/sha256.log

>@echo "[M10] audit PASS"

.PHONY: m10-all
m10-all: m10-host-test m10-freestanding m10-audit
>bash scripts/m10_preflight.sh
>@echo "======================================"
>@echo "[M10] syscall milestone PASS"
>@echo "======================================"
