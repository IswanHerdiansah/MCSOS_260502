#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/build/evidence/M0"

mkdir -p "$OUT"

echo "[+] collecting metadata"
cp -r "$ROOT/build/meta" "$OUT/" 2>/dev/null || true

echo "[+] collecting smoke outputs"
cp -r "$ROOT/build/smoke" "$OUT/" 2>/dev/null || true

echo "[+] collecting git summary"
git status > "$OUT/git-status.txt"
git log --oneline -5 > "$OUT/git-log.txt"

echo "[+] collecting host info"
uname -a > "$OUT/uname.txt"

echo "OK: evidence collected in $OUT"
