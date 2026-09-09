#!/usr/bin/env bash
#
# Build useTracker as an AppImage: a single executable file that carries its own
# OpenCV, FFmpeg and SDL2 and runs on any distribution with a glibc at least as
# old as the one used here.
#
#   ./packaging/linux/build-appimage.sh
#
# Build it on the OLDEST distribution you want to support — an AppImage built on
# Ubuntu 22.04 runs on Ubuntu 22.04+, Debian 12+, Fedora 36+, Manjaro and Arch,
# but one built on Arch will not run on Ubuntu. packaging/linux/build-in-docker.sh
# does exactly that in a container, and needs nothing installed but Docker.
#
# linuxdeploy is downloaded into the build directory on first use.

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

BUILD_DIR="${BUILD_DIR:-build-appimage}"
APPDIR="$BUILD_DIR/AppDir"
DIST_DIR="${DIST_DIR:-dist}"
TOOLS_DIR="$BUILD_DIR/tools"
ARCH="$(uname -m)"

# The single source of truth for the version is project(... VERSION x.y.z) in CMakeLists.txt
VERSION="$(sed -n 's/^[[:space:]]*VERSION[[:space:]]\{1,\}\([0-9][0-9.]*\).*/\1/p' CMakeLists.txt | head -1)"
VERSION="${VERSION:-0.0.0}"

# linuxdeploy itself is an AppImage; extracting it avoids needing FUSE, which is
# unavailable in most containers and CI runners.
export APPIMAGE_EXTRACT_AND_RUN=1
export ARCH

fetch() {  # fetch <url> <destination>
    local url="$1" dest="$2"
    [[ -x "$dest" ]] && return 0
    echo ">> downloading $(basename "$dest")"
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL "$url" -o "$dest"
    elif command -v wget >/dev/null 2>&1; then
        wget -q "$url" -O "$dest"
    else
        echo "error: need curl or wget to download linuxdeploy" >&2
        exit 1
    fi
    chmod +x "$dest"
}

mkdir -p "$TOOLS_DIR" "$DIST_DIR"

LINUXDEPLOY="$TOOLS_DIR/linuxdeploy-$ARCH.AppImage"
PLUGIN="$TOOLS_DIR/linuxdeploy-plugin-appimage-$ARCH.AppImage"
BASE_URL="https://github.com/linuxdeploy"
fetch "$BASE_URL/linuxdeploy/releases/download/continuous/linuxdeploy-$ARCH.AppImage" "$LINUXDEPLOY"
fetch "$BASE_URL/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-$ARCH.AppImage" "$PLUGIN"

# --- build and stage --------------------------------------------------------
echo ">> configuring"
cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr

echo ">> building"
cmake --build "$BUILD_DIR" --parallel "$(nproc)"

echo ">> staging into $APPDIR"
rm -rf "$APPDIR"
DESTDIR="$PWD/$APPDIR" cmake --install "$BUILD_DIR"

# --- collect dependencies and pack -----------------------------------------
echo ">> collecting shared libraries"
"$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --executable "$APPDIR/usr/bin/useTracker" \
    --desktop-file "$APPDIR/usr/share/applications/useTracker.desktop" \
    --icon-file packaging/icons/useTracker-256.png \
    --icon-filename useTracker

# OpenCV's videoio can dlopen this backend rather than link it; carry it along
# when the distribution ships it separately.
for extra in /usr/lib/*/libopencv_videoio_ffmpeg*.so /usr/lib/libopencv_videoio_ffmpeg*.so; do
    [[ -e "$extra" ]] && cp -n "$extra" "$APPDIR/usr/lib/" || true
done

echo ">> packing AppImage"
OUTPUT="useTracker-$VERSION-$ARCH.AppImage" \
    "$PLUGIN" --appdir "$APPDIR"

mv -f "useTracker-$VERSION-$ARCH.AppImage" "$DIST_DIR/"
chmod +x "$DIST_DIR/useTracker-$VERSION-$ARCH.AppImage"

echo
echo ">> $DIST_DIR/useTracker-$VERSION-$ARCH.AppImage"
echo "   run it with:  ./$DIST_DIR/useTracker-$VERSION-$ARCH.AppImage"
