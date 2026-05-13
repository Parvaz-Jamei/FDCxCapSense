#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
grep -R "DirectFDC1004CapacitanceRequired" -n src extras/tests >/dev/null
! grep -R "CapdacManager" -n src/devices/fdc2x1x src/calibration/fdc2x1x
echo "PASS: architecture contract scan"
