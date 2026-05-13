#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
FQBN="${1:-arduino:avr:uno}"
if ! command -v arduino-cli >/dev/null 2>&1; then
  echo "Arduino CLI compile: NOT RUN - arduino-cli unavailable in this environment"
  if [ "${ALLOW_NO_ARDUINO_CLI:-0}" = "1" ]; then
    exit 0
  fi
  exit 2
fi
status=0
for example in examples/*; do
  [ -d "$example" ] || continue
  ino="$example/$(basename "$example").ino"
  if [ -f "$ino" ]; then
    echo "Compiling $ino for $FQBN"
    arduino-cli compile --fqbn "$FQBN" --library . "$example" || status=$?
  fi
done
exit "$status"
