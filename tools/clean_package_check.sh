#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
found=$(find . -type f \( -name "*.o" -o -name "*.a" -o -name "*.so" -o -name "*.elf" -o -name "*.hex" -o -name "*.bin" -o -name "*.map" -o -name "*.d" \))
if [ -n "$found" ]; then
  echo "$found"
  echo "FAIL: generated build artifacts are present" >&2
  exit 1
fi
echo "PASS: no generated build artifacts found"
