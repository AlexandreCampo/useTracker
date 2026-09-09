#!/usr/bin/env bash
#
# Build useTracker.app and wrap it in a .dmg.
#
#   ./packaging/macos/build-macos.sh
#
# The bundle carries copies of the Homebrew OpenCV, FFmpeg and SDL2 dylibs, so
# the .dmg runs on a Mac without Homebrew. It is built for the architecture of
# this machine (arm64 on Apple Silicon, x86_64 on Intel) and for macOS versions
# at least as new as this one — Homebrew bottles are not built for older systems.
#
# The app is ad-hoc signed, which is what lets it launch at all on Apple Silicon
# after its dylib paths are rewritten. It is not notarised: on first launch,
# right-click the app and choose Open, or run
#     xattr -dr com.apple.quarantine /Applications/useTracker.app
#
# Set SIGN_IDENTITY to a Developer ID certificate name to sign properly.

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

BUILD_DIR="${BUILD_DIR:-build-macos}"
DIST_DIR="${DIST_DIR:-dist}"
SIGN_IDENTITY="${SIGN_IDENTITY:--}"   # "-" means ad-hoc

[[ "$(uname -s)" == "Darwin" ]] || { echo "error: this script only runs on macOS" >&2; exit 1; }

command -v brew >/dev/null 2>&1 || { echo "error: Homebrew is required — https://brew.sh" >&2; exit 1; }

missing=()
for formula in cmake pkg-config opencv sdl2 ffmpeg; do
    brew list --formula "$formula" >/dev/null 2>&1 || missing+=("$formula")
done
if (( ${#missing[@]} )); then
    echo "error: missing Homebrew formulae: ${missing[*]}"
    echo "       run ./scripts/install-deps.sh"
    exit 1
fi

# Homebrew installs to /opt/homebrew on Apple Silicon and /usr/local on Intel.
BREW_PREFIX="$(brew --prefix)"

echo ">> configuring"
cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$BREW_PREFIX" \
    -DUSETRACKER_BUNDLE_DEPS=ON \
    -DUSETRACKER_MACOS_SIGN_IDENTITY="$SIGN_IDENTITY"

echo ">> building"
cmake --build "$BUILD_DIR" --parallel "$(sysctl -n hw.ncpu)"

echo ">> packaging"
mkdir -p "$DIST_DIR"
( cd "$BUILD_DIR" && cpack -G "DragNDrop;TGZ" )

find "$BUILD_DIR" -maxdepth 1 \( -name '*.dmg' -o -name 'useTracker-*.tar.gz' \) \
    -exec mv -f {} "$DIST_DIR/" \;

echo
echo ">> artifacts:"
ls -1 "$DIST_DIR"
