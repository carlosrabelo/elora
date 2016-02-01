#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../elora" && pwd)"
SRC_DIR="$ROOT_DIR/src"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++14 -Wall -Wextra -Wpedantic -I$SRC_DIR"

find "$SRC_DIR" -name '*.cpp' -print0 | xargs -0 -I {} $CXX $CXXFLAGS -fsyntax-only {}
echo "Syntax OK"
