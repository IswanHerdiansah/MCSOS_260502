#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT="$ROOT/build/archive"
mkdir -p "$OUT"

ARCHIVE="$OUT/m1-evidence-$(date -u +%Y%m%dT%H%M%SZ).tar.gz"

echo "[M1] Archiving evidence..."

tar -czf "$ARCHIVE" \
  -C "$ROOT" \
  build/meta \
  build/proof \
  build/repro

echo "OK: evidence archived at $ARCHIVE"
