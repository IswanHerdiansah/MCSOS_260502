#!/usr/bin/env bash

set -euo pipefail

mkdir -p build/m8

make m8-all

echo "[M8] static grade PASS"
