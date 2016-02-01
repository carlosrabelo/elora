#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../elora" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
TEST_DIR="$ROOT_DIR/tests"
LIB_DIR="$ROOT_DIR/lib"
BUILD_DIR="$REPO_ROOT/bin"
BINARY_NAME="${BINARY_NAME:-elora}"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++14 -Wall -Wextra -Wpedantic -I$SRC_DIR -I$LIB_DIR"
X11_LIBS="$(pkg-config --libs x11 2>/dev/null || echo -lX11)"

if [ ! -f "$LIB_DIR/catch.hpp" ]; then
    echo "Catch2 header not found. Run 'make setup' first."
    exit 1
fi

mkdir -p "$BUILD_DIR"
mapfile -t TEST_SRCS < <(find "$TEST_DIR" -maxdepth 1 -name '*.cpp' -print | LC_ALL=C sort)
mapfile -t LIB_SRCS < <(find "$SRC_DIR" -name '*.cpp' ! -name 'main.cpp' -print | LC_ALL=C sort)
# shellcheck disable=SC2086
$CXX $CXXFLAGS "${TEST_SRCS[@]}" "${LIB_SRCS[@]}" $X11_LIBS -o "$BUILD_DIR/${BINARY_NAME}-tests"
echo "Running tests..."
exec "$BUILD_DIR/${BINARY_NAME}-tests" "$@"
