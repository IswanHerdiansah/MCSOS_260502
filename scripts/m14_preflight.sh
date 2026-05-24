#!/usr/bin/env bash
set -euo pipefail

mkdir -p evidence/M14

LOG="evidence/M14/preflight.log"

: > "$LOG"

require_cmd() {
    local cmd="$1"

    if ! command -v "$cmd" >/dev/null 2>&1; then
        echo "MISSING_CMD: $cmd" | tee -a "$LOG"
        exit 1
    fi

    echo "OK_CMD: $cmd" | tee -a "$LOG"
}

require_cmd clang
require_cmd ld.lld
require_cmd nm
require_cmd readelf
require_cmd objdump
require_cmd sha256sum
require_cmd qemu-system-x86_64

for d in kernel tests scripts; do
    if [[ -d "$d" ]]; then
        echo "OK_DIR: $d" | tee -a "$LOG"
    else
        echo "MISSING_DIR: $d" | tee -a "$LOG"
        exit 1
    fi
done

echo "M14_PREFLIGHT_DONE" | tee -a "$LOG"
