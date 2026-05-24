#!/usr/bin/env bash
set -euo pipefail

echo "[M12] Preflight synchronization subsystem"

for tool in git make clang nm readelf objdump sha256sum; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "[FAIL] missing tool: $tool"
        exit 1
    fi

    echo "[OK] $tool -> $(command -v "$tool")"
done

required_dirs=(
    kernel
    kernel/include
    kernel/sync
    tests
    build
)

for d in "${required_dirs[@]}"; do
    if [ -d "$d" ]; then
        echo "[OK] directory exists: $d"
    else
        echo "[WARN] missing directory: $d"
    fi
done

required_markers=(
    panic
    syscall
    thread
    sched
    kmem
)

for m in "${required_markers[@]}"; do
    if grep -R "$m" kernel kernel/include >/dev/null 2>&1; then
        echo "[OK] marker found: $m"
    else
        echo "[WARN] marker missing: $m"
    fi
done

echo "[OK] commit: $(git rev-parse --short HEAD)"
git status --short
