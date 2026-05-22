#!/usr/bin/env bash

set -euo pipefail

mkdir -p build/m8/evidence

make m8-all \
    2>&1 | tee build/m8/evidence/m8_make.log

cp build/m8/readelf_h.txt \
   build/m8/evidence/

cp build/m8/kmem.objdump.txt \
   build/m8/evidence/

cp build/m8/nm_u.txt \
   build/m8/evidence/

echo "[PASS] M8 grade selesai"
