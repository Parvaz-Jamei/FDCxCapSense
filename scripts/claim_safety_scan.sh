#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
exec bash ./tools/claim_safety_scan.sh
