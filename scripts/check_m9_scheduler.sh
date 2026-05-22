#!/usr/bin/env bash
set -euo pipefail

printf '[M9] checking scheduler files...\n'

required=(
  kernel/include/mcsos/kernel/thread.h
  kernel/core/thread.c
  kernel/arch/x86_64/context_switch.S
  tests/test_scheduler.c
)

for f in "${required[@]}"; do
    if [[ ! -f "$f" ]]; then
        printf '[FAIL] missing %s\n' "$f"
        exit 1
    fi
done

printf '[M9] host test...\n'

make m9-host-test

printf '[M9] audit...\n'

make m9-audit

printf '[PASS] M9 scheduler validated.\n'
