#!/usr/bin/env bash
#
# Configure, build and optionally package useTracker on Linux or macOS.
#
#   ./scripts/build.sh                  # release build into build/
#   ./scripts/build.sh --install        # ... and install it system-wide
#   ./scripts/build.sh --package        # ... and write .tar.gz/.deb/.rpm/.dmg to dist/
#   ./scripts/build.sh -t Debug -j 4    # debug build, 4 jobs
#
# For a binary that runs on any distribution, use packaging/linux/build-appimage.sh
# (or packaging/linux/build-in-docker.sh, which needs nothing installed but Docker).

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

BUILD_TYPE=Release
BUILD_DIR=build
DIST_DIR=dist
PREFIX=""
JOBS=""
DO_INSTALL=0
DO_PACKAGE=0
DO_CLEAN=0
EXTRA_CMAKE=()

usage() {
    cat <<'EOF'
Build useTracker.

Usage: scripts/build.sh [options] [-- <extra cmake args>]

Options:
  -t, --type TYPE      Release (default), Debug, RelWithDebInfo, MinSizeRel
  -b, --build-dir DIR  build directory (default: build)
  -p, --prefix DIR     install prefix (default: /usr/local, or CMake's default)
  -j, --jobs N         parallel jobs (default: all cores)
      --install        install after building (uses sudo when the prefix needs it)
      --package        run CPack and collect the artifacts in dist/
      --bluetooth      enable the optional Bluetooth remote control
      --clean          delete the build directory first
  -h, --help           this message
EOF
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -t|--type)      BUILD_TYPE="$2"; shift ;;
        -b|--build-dir) BUILD_DIR="$2";  shift ;;
        -p|--prefix)    PREFIX="$2";     shift ;;
        -j|--jobs)      JOBS="$2";       shift ;;
        --install)      DO_INSTALL=1 ;;
        --package)      DO_PACKAGE=1 ;;
        --bluetooth)    EXTRA_CMAKE+=(-DUSE_BLUETOOTH=ON) ;;
        --clean)        DO_CLEAN=1 ;;
        -h|--help)      usage ;;
        --)             shift; EXTRA_CMAKE+=("$@"); break ;;
        *) echo "unknown option: $1 (try --help)" >&2; exit 2 ;;
    esac
    shift
done

command -v cmake >/dev/null 2>&1 \
    || { echo "error: cmake not found — run ./scripts/install-deps.sh" >&2; exit 1; }

if [[ -z "$JOBS" ]]; then
    JOBS="$( (command -v nproc >/dev/null 2>&1 && nproc) \
             || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
fi

if (( DO_CLEAN )) && [[ -d "$BUILD_DIR" ]]; then
    echo ">> removing $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

CONFIGURE=(cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE")
[[ -n "$PREFIX" ]] && CONFIGURE+=(-DCMAKE_INSTALL_PREFIX="$PREFIX")
CONFIGURE+=("${EXTRA_CMAKE[@]+"${EXTRA_CMAKE[@]}"}")

echo ">> configuring ($BUILD_TYPE)"
"${CONFIGURE[@]}"

echo ">> building with $JOBS jobs"
cmake --build "$BUILD_DIR" --parallel "$JOBS"

echo
echo ">> built $BUILD_DIR/useTracker"

if (( DO_PACKAGE )); then
    echo ">> packaging"
    mkdir -p "$DIST_DIR"
    ( cd "$BUILD_DIR" && cpack )
    find "$BUILD_DIR" -maxdepth 1 \
        \( -name 'useTracker-*.tar.gz' -o -name '*.deb' -o -name '*.rpm' \
           -o -name '*.dmg' -o -name '*.zip' \) \
        -exec mv -f {} "$DIST_DIR/" \;
    echo ">> artifacts in $DIST_DIR:"
    ls -1 "$DIST_DIR"
fi

if (( DO_INSTALL )); then
    install_prefix="$(cmake -L -N "$BUILD_DIR" \
        | sed -n 's/^CMAKE_INSTALL_PREFIX:PATH=//p')"
    echo ">> installing to ${install_prefix:-<cmake default>}"

    SUDO=""
    if [[ $EUID -ne 0 && ! -w "${install_prefix:-/usr/local}" ]]; then
        SUDO=sudo
    fi
    $SUDO cmake --install "$BUILD_DIR"

    # Refresh the desktop database so the launcher entry shows up right away.
    if command -v update-desktop-database >/dev/null 2>&1; then
        $SUDO update-desktop-database "${install_prefix}/share/applications" 2>/dev/null || true
    fi
fi
