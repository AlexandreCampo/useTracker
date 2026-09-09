#!/usr/bin/env bash
#
# Install everything useTracker needs to build, on whatever this machine runs.
#
#   ./scripts/install-deps.sh              # show the command, ask, then run it
#   ./scripts/install-deps.sh -y           # no questions
#   ./scripts/install-deps.sh --dry-run    # just print the command
#
# Supported: Debian/Ubuntu/Mint, Arch/Manjaro/EndeavourOS, Fedora/RHEL,
#            openSUSE, macOS (Homebrew).
#
# Everything else — Dear ImGui, CLI11, portable-file-dialogs — is fetched by
# CMake at configure time and needs no system package.

set -euo pipefail

ASSUME_YES=0
DRY_RUN=0
WITH_BLUETOOTH=0

usage() {
    cat <<'EOF'
Install the system packages useTracker needs to build.

Usage: scripts/install-deps.sh [options]

Options:
  -y, --yes         do not ask for confirmation
  -n, --dry-run     print the install command and exit
      --bluetooth   also install libbluetooth (for the optional remote control)
  -h, --help        this message

Supported: Debian/Ubuntu/Mint, Arch/Manjaro/EndeavourOS, Fedora/RHEL,
           openSUSE, macOS (Homebrew).
EOF
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -y|--yes)     ASSUME_YES=1 ;;
        -n|--dry-run) DRY_RUN=1 ;;
        --bluetooth)  WITH_BLUETOOTH=1 ;;
        -h|--help)    usage ;;
        *) echo "unknown option: $1 (try --help)" >&2; exit 2 ;;
    esac
    shift
done

die() { echo "error: $*" >&2; exit 1; }

# --- pick a package manager -------------------------------------------------
if [[ "$(uname -s)" == "Darwin" ]]; then
    MANAGER=brew
elif command -v apt-get >/dev/null 2>&1; then
    MANAGER=apt
elif command -v pacman >/dev/null 2>&1; then
    MANAGER=pacman
elif command -v dnf >/dev/null 2>&1; then
    MANAGER=dnf
elif command -v zypper >/dev/null 2>&1; then
    MANAGER=zypper
else
    die "no supported package manager found — see the Building section of README.md"
fi

SUDO=""
if [[ "$MANAGER" != "brew" && $EUID -ne 0 ]]; then
    command -v sudo >/dev/null 2>&1 || die "need root privileges; install sudo or run as root"
    SUDO=sudo
fi

# --- package lists ----------------------------------------------------------
case "$MANAGER" in
apt)
    PACKAGES=(
        build-essential cmake git pkg-config
        libopencv-dev libopencv-contrib-dev
        libsdl2-dev libgl1-mesa-dev
        libavformat-dev libavcodec-dev libswscale-dev libavutil-dev
    )
    (( WITH_BLUETOOTH )) && PACKAGES+=(libbluetooth-dev)
    INSTALL=($SUDO apt-get install -y "${PACKAGES[@]}")
    REFRESH=($SUDO apt-get update)
    ;;
pacman)
    # sdl2 was renamed sdl2-compat on current Arch; the availability filter
    # below keeps this working on both.
    PACKAGES=(base-devel cmake git pkgconf opencv sdl2-compat sdl2 ffmpeg glu vtk hdf5)
    (( WITH_BLUETOOTH )) && PACKAGES+=(bluez-libs)
    INSTALL=($SUDO pacman -S --needed --noconfirm)
    REFRESH=($SUDO pacman -Sy)
    ;;
dnf)
    PACKAGES=(
        gcc-c++ cmake git pkgconf-pkg-config
        opencv-devel SDL2-devel mesa-libGL-devel
        ffmpeg-devel
    )
    (( WITH_BLUETOOTH )) && PACKAGES+=(bluez-libs-devel)
    INSTALL=($SUDO dnf install -y "${PACKAGES[@]}")
    REFRESH=(true)
    echo "note: ffmpeg-devel on Fedora comes from RPM Fusion —"
    echo "      https://rpmfusion.org/Configuration if the package is not found."
    ;;
zypper)
    PACKAGES=(
        gcc-c++ cmake git pkg-config
        opencv-devel libSDL2-devel Mesa-libGL-devel
        libavformat-devel libavcodec-devel libswscale-devel libavutil-devel
    )
    (( WITH_BLUETOOTH )) && PACKAGES+=(bluez-devel)
    INSTALL=($SUDO zypper install -y "${PACKAGES[@]}")
    REFRESH=(true)
    echo "note: the FFmpeg devel packages come from Packman on openSUSE."
    ;;
brew)
    command -v brew >/dev/null 2>&1 \
        || die "Homebrew is not installed — see https://brew.sh"
    PACKAGES=(cmake pkg-config opencv sdl2 ffmpeg)
    INSTALL=(brew install "${PACKAGES[@]}")
    REFRESH=(brew update)
    ;;
esac

# On Arch a package may have been renamed; drop the ones the repos do not know.
if [[ "$MANAGER" == "pacman" ]]; then
    available=()
    for pkg in "${PACKAGES[@]}"; do
        if pacman -Si "$pkg" >/dev/null 2>&1; then available+=("$pkg"); fi
    done
    [[ ${#available[@]} -gt 0 ]] || die "none of the expected packages exist in your repositories"
    INSTALL+=("${available[@]}")
fi

# --- run --------------------------------------------------------------------
echo
echo "Package manager : $MANAGER"
echo "Command         : ${INSTALL[*]}"
echo

if (( DRY_RUN )); then exit 0; fi

if (( ! ASSUME_YES )); then
    read -r -p "Install these packages? [y/N] " reply
    [[ "$reply" =~ ^[Yy]$ ]] || { echo "aborted."; exit 1; }
fi

"${REFRESH[@]}"
"${INSTALL[@]}"

echo
echo "Done. Build with:  ./scripts/build.sh"
