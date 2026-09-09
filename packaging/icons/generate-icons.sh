#!/usr/bin/env bash
# Regenerate every raster icon from useTracker.svg.
#
# The generated files are committed so that building useTracker never needs a
# rasteriser; run this only after editing the SVG.
#
# Requires: rsvg-convert (librsvg) or inkscape, ImageMagick, python3.

set -euo pipefail
cd "$(dirname "$0")"

SIZES_PNG=(64 128 256 512)          # shipped for the hicolor icon theme
SIZES_ICNS=(16 32 64 128 256 512 1024)
SIZES_ICO=(16 32 48 64 128 256)

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

render() {  # render <size> <output>
    if command -v rsvg-convert >/dev/null 2>&1; then
        rsvg-convert -w "$1" -h "$1" useTracker.svg -o "$2"
    elif command -v inkscape >/dev/null 2>&1; then
        inkscape useTracker.svg -w "$1" -h "$1" -o "$2" >/dev/null 2>&1
    else
        echo "error: need rsvg-convert or inkscape" >&2
        exit 1
    fi
}

all_sizes=$(printf '%s\n' "${SIZES_PNG[@]}" "${SIZES_ICNS[@]}" "${SIZES_ICO[@]}" | sort -un)
for size in $all_sizes; do
    render "$size" "$tmp/icon-$size.png"
done

# --- Linux: hicolor theme PNGs ---------------------------------------------
for size in "${SIZES_PNG[@]}"; do
    cp "$tmp/icon-$size.png" "useTracker-$size.png"
done

# --- macOS: .icns ----------------------------------------------------------
icns_inputs=()
for size in "${SIZES_ICNS[@]}"; do icns_inputs+=("$tmp/icon-$size.png"); done
if command -v iconutil >/dev/null 2>&1; then
    # Prefer the native tool when running on macOS.
    set="$tmp/useTracker.iconset"; mkdir -p "$set"
    cp "$tmp/icon-16.png"   "$set/icon_16x16.png"
    cp "$tmp/icon-32.png"   "$set/icon_16x16@2x.png"
    cp "$tmp/icon-32.png"   "$set/icon_32x32.png"
    cp "$tmp/icon-64.png"   "$set/icon_32x32@2x.png"
    cp "$tmp/icon-128.png"  "$set/icon_128x128.png"
    cp "$tmp/icon-256.png"  "$set/icon_128x128@2x.png"
    cp "$tmp/icon-256.png"  "$set/icon_256x256.png"
    cp "$tmp/icon-512.png"  "$set/icon_256x256@2x.png"
    cp "$tmp/icon-512.png"  "$set/icon_512x512.png"
    cp "$tmp/icon-1024.png" "$set/icon_512x512@2x.png"
    iconutil -c icns "$set" -o useTracker.icns
else
    python3 make-icns.py useTracker.icns "${icns_inputs[@]}"
fi

# --- Windows: .ico ---------------------------------------------------------
ico_inputs=()
for size in "${SIZES_ICO[@]}"; do ico_inputs+=("$tmp/icon-$size.png"); done
magick_bin=$(command -v magick || command -v convert || true)
if [[ -z "$magick_bin" ]]; then
    echo "error: need ImageMagick for the .ico" >&2
    exit 1
fi
"$magick_bin" "${ico_inputs[@]}" useTracker.ico

ls -la useTracker.svg useTracker-*.png useTracker.icns useTracker.ico
