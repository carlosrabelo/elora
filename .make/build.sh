#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../elora" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
BUILD_DIR="$REPO_ROOT/bin"
BINARY_NAME="${BINARY_NAME:-elora}"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++14 -Wall -Wextra -Wpedantic -I$SRC_DIR"
X11_LIBS="$(pkg-config --libs x11 2>/dev/null || echo -lX11)"

mkdir -p "$BUILD_DIR"
echo "Building $BINARY_NAME..."
mapfile -t SRCS < <(find "$SRC_DIR" -name '*.cpp' -print | LC_ALL=C sort)
# shellcheck disable=SC2086
$CXX $CXXFLAGS "${SRCS[@]}" $X11_LIBS -o "$BUILD_DIR/$BINARY_NAME"
echo "Done: $BUILD_DIR/$BINARY_NAME"
