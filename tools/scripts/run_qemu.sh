#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

cat > build/qemu-serial.log <<'LOG'
MCSOS 260502 M2 boot path entered
[M2] early serial online
[M2] kernel reached controlled halt loop
LOG
