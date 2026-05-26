#!/usr/bin/env bash
set -euo pipefail

mkdir -p evidence/M16

{
  echo "== git =="

  git status --short || true
  git rev-parse --short HEAD || true

  echo "== toolchain =="

clang --version | sed -n '1p'
ld.lld --version | sed -n '1p'
nm --version | sed -n '1p'
readelf --version | sed -n '1p'
objdump --version | sed -n '1p'
make --version | sed -n '1p'

  echo "== prior evidence =="

  for d in \
    M3 \
    M4 \
    M12 \
    M13 \
    M14 \
    M15
  do
    if [ -d "evidence/$d" ]; then
      echo "evidence/$d: present"
    else
      echo "evidence/$d: missing"
    fi
  done

  echo "M16_PREFLIGHT_DONE"

} | tee evidence/M16/m16_preflight.log
