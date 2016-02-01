#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$REPO_ROOT/bin"
BINARY_NAME="${BINARY_NAME:-elora}"

if [ ! -f "$BUILD_DIR/$BINARY_NAME" ]; then
    echo "Binary not found. Run 'make build' first."
    exit 1
fi

exec "$BUILD_DIR/$BINARY_NAME" "$@"
