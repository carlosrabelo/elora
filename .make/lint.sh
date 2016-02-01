#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../elora" && pwd)"
SRC_DIR="$ROOT_DIR/src"

if ! command -v cppcheck &>/dev/null; then
    echo "error: cppcheck not found" >&2
    echo "hint:  install cppcheck to run 'make lint'" >&2
    exit 1
fi

cppcheck --std=c++14 --enable=warning,style,performance,portability \
    --error-exitcode=1 \
    -I "$SRC_DIR" \
    --suppress=missingIncludeSystem \
    --suppress=noExplicitConstructor:"$SRC_DIR/core/optional.hpp" \
    --suppress=noExplicitConstructor:"$SRC_DIR/core/span.hpp" \
    "$SRC_DIR"
