#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
exec bash ./tools/clean_package_check.sh
