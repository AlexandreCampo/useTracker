#!/usr/bin/env bash
#
# Build the redistributable Linux artifacts inside a container, so the result is
# reproducible and links against an old enough glibc to run everywhere.
#
#   ./packaging/linux/build-in-docker.sh              # AppImage + .deb
#   ./packaging/linux/build-in-docker.sh --appimage   # AppImage only
#   ./packaging/linux/build-in-docker.sh --image ubuntu:24.04
#
# Nothing but Docker (or Podman) is needed on the host — this is the way to
# produce Linux binaries from a rolling-release machine whose OpenCV is newer
# than what the rest of the world runs.
#
# Artifacts land in dist/.

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

IMAGE="ubuntu:22.04"
WANT_APPIMAGE=1
WANT_DEB=1

usage() {
    cat <<'EOF'
Build useTracker's Linux artifacts in a container.

Usage: packaging/linux/build-in-docker.sh [options]

Options:
      --image IMAGE   base image (default: ubuntu:22.04)
      --appimage      build only the AppImage
      --deb           build only the .deb
  -h, --help          this message
EOF
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --image)    IMAGE="$2"; shift ;;
        --appimage) WANT_DEB=0 ;;
        --deb)      WANT_APPIMAGE=0 ;;
        -h|--help)  usage ;;
        *) echo "unknown option: $1 (try --help)" >&2; exit 2 ;;
    esac
    shift
done

if command -v docker >/dev/null 2>&1; then
    ENGINE=docker
elif command -v podman >/dev/null 2>&1; then
    ENGINE=podman
else
    echo "error: neither docker nor podman is installed" >&2
    exit 1
fi

echo ">> building in $IMAGE via $ENGINE"

# The build runs as root inside the container (apt needs it) and hands the
# artifacts back with the host user's ownership at the end.
"$ENGINE" run --rm \
    -v "$ROOT:/src" -w /src \
    -e APPIMAGE_EXTRACT_AND_RUN=1 \
    -e DEBIAN_FRONTEND=noninteractive \
    -e HOST_UID="$(id -u)" -e HOST_GID="$(id -g)" \
    -e WANT_APPIMAGE="$WANT_APPIMAGE" -e WANT_DEB="$WANT_DEB" \
    "$IMAGE" bash -euo pipefail -c '
        # Hand whatever was produced back to the host user, including when the
        # build fails partway through — root-owned leftovers in the source tree
        # are painful to clean up.
        trap "chown -R \"\$HOST_UID:\$HOST_GID\" dist build-docker-appimage build-docker-deb 2>/dev/null || true" EXIT

        apt-get update -qq
        apt-get install -y -qq --no-install-recommends \
            ca-certificates curl file git sudo desktop-file-utils >/dev/null

        ./scripts/install-deps.sh -y

        if [ "$WANT_APPIMAGE" = 1 ]; then
            BUILD_DIR=build-docker-appimage ./packaging/linux/build-appimage.sh
        fi

        if [ "$WANT_DEB" = 1 ]; then
            ./scripts/build.sh --build-dir build-docker-deb --package
        fi
    '

echo
echo ">> artifacts:"
ls -1 dist/
