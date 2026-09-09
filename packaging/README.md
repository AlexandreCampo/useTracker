# Packaging useTracker

Everything needed to turn the source tree into a binary someone else can run.

```
packaging/
├── arch/PKGBUILD             Arch / Manjaro / EndeavourOS native package
├── icons/                    the app icon, and the script that regenerates it
├── linux/
│   ├── build-appimage.sh     one file that runs on every distribution
│   ├── build-in-docker.sh    the same, in a container — needs only Docker
│   ├── useTracker.desktop    launcher entry
│   └── org.usetracker.useTracker.metainfo.xml   AppStream metadata
├── macos/
│   ├── build-macos.sh        useTracker.app inside a .dmg
│   └── Info.plist.in         bundle metadata template
└── windows/
    ├── build-windows.ps1     .zip and NSIS installer, dependencies from vcpkg
    └── useTracker.rc         embeds the icon in the .exe
```

The `.deb`, `.rpm`, `.tar.gz`, `.dmg`, `.zip` and NSIS installer all come from
CPack, configured in [`../cmake/Packaging.cmake`](../cmake/Packaging.cmake) and
driven by `scripts/build.sh --package`. The AppImage is the one artifact CPack
does not produce, because bundling libraries correctly needs `linuxdeploy`.

## What to build for whom

| Target | Artifact | Command |
|---|---|---|
| Any Linux, no install | `.AppImage` | `packaging/linux/build-appimage.sh` |
| Ubuntu / Debian | `.deb` | `scripts/build.sh --package` |
| Fedora / openSUSE | `.rpm` | `scripts/build.sh --package` (needs `rpmbuild`) |
| Arch / Manjaro | `.pkg.tar.zst` | `cd packaging/arch && makepkg -si` |
| macOS | `.dmg` | `packaging/macos/build-macos.sh` |
| Windows | `.zip` + installer | `packaging\windows\build-windows.ps1` |

All artifacts land in `dist/`.

## Linux: which distribution to build on

A binary linked on Arch will not start on Ubuntu — its glibc is newer. The rule
is to build on the **oldest** system you intend to support:

```bash
# From any machine, including a rolling-release one. Needs only Docker.
./packaging/linux/build-in-docker.sh
```

This produces an AppImage built against Ubuntu 22.04 (glibc 2.35), which covers
Ubuntu 22.04+, Debian 12+, Fedora 36+, openSUSE Leap 15.5+, Arch and Manjaro,
plus a `.deb` for Ubuntu 22.04. Pass `--image ubuntu:24.04` for a 24.04 `.deb`.

Building the AppImage natively (`packaging/linux/build-appimage.sh`) is fine when
your machine is the oldest system you care about.

## macOS notes

The `.dmg` is not notarised. On first launch macOS will refuse to open it; either
right-click the app and choose *Open*, or:

```bash
xattr -dr com.apple.quarantine /Applications/useTracker.app
```

The bundle is ad-hoc signed during install — without that it would not launch at
all on Apple Silicon, because `fixup_bundle` rewrites the dylib paths and
invalidates the linker's signature. To sign with a real certificate:

```bash
SIGN_IDENTITY="Developer ID Application: Your Name (TEAMID)" ./packaging/macos/build-macos.sh
```

Homebrew bottles are built for the running macOS version, so a `.dmg` built on
macOS 14 will not run on macOS 12. Build on the oldest macOS you support.

## Windows notes

The first build compiles OpenCV from source through vcpkg and takes about an
hour; set `VCPKG_ROOT` to an existing vcpkg checkout, or let the script clone one
into the build directory. The `.zip` carries every DLL next to `useTracker.exe`.

`useTracker.exe` is a console application on purpose: the `--nogui` batch mode
must be able to print. That means a console window appears alongside the GUI.

## Continuous integration

[`.github/workflows/build.yml`](../.github/workflows/build.yml) builds all of the
above on every push, and attaches them to a GitHub Release when a `v*` tag is
pushed:

```bash
git tag -a v2.0.0 -m "useTracker 2.0.0"
git push origin v2.0.0
```

Pull requests only run the Linux job; macOS and Windows run on branch pushes,
tags and manual dispatch.

## Releasing

1. Bump `VERSION` in the top-level `CMakeLists.txt` — it is the single source of
   truth. `vcpkg.json`, `packaging/arch/PKGBUILD` and the AppStream `<releases>`
   entry carry their own copies; update those too.
2. Push the `v<version>` tag and let CI publish the artifacts.
3. For the AUR, regenerate the checksum in the PKGBUILD with `makepkg -g`.

## Icons

`icons/useTracker.svg` is the master. After editing it:

```bash
./packaging/icons/generate-icons.sh
```

This rewrites the hicolor PNGs, the Windows `.ico` and the macOS `.icns` (via
`iconutil` on macOS, or `make-icns.py` elsewhere). The generated files are
committed so that building useTracker never requires a rasteriser.
