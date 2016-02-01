#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if command -v clang-format &>/dev/null; then
    echo "=== Format ==="
    "$SCRIPT_DIR/fmt.sh"
fi

if command -v cppcheck &>/dev/null; then
    echo "=== Lint ==="
    "$SCRIPT_DIR/lint.sh"
fi

echo "=== Test ==="
"$SCRIPT_DIR/test.sh"

echo "=== Quality OK ==="
