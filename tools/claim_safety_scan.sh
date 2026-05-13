#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
# Negative disclaimers are allowed. This scan targets positive claim phrases.
forbidden='black ice detected|safe road|unsafe road|certified ice|automotive-grade|ASIL|guaranteed detection|field-validated profile|field-validated|safety trigger|actuator control output|actuator output|product-ready detection|product-ready'
allow_negative='not |no |does not|do not|forbidden|negative/disclaimer|negative disclaimers|claim limits|without |unless independently validated|must not|is not|not as proof'
TMP=$(mktemp)
trap 'rm -f "$TMP"' EXIT
if grep -RInE "$forbidden" README.md docs examples src extras/tests 2>/dev/null | grep -viE "$allow_negative" > "$TMP"; then
  cat "$TMP"
  echo "FAIL: positive forbidden claim phrase found" >&2
  exit 1
fi
echo "PASS: no positive forbidden claim phrases found; negative disclaimers allowed"
