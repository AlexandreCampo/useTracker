---
title: Install
description: Downloading a package or building useTracker from source, with platform notes and dependencies.
---

Check the [GitHub releases page](https://github.com/AlexandreCampo/useTracker/releases)
for prebuilt packages. The project includes packaging for the formats below;
availability depends on the release. If there is no package for your platform,
build from source using the instructions further down this page.

## Packages

| System | Format | Getting started |
| --- | --- | --- |
| Linux | `.AppImage` | Make the file executable, then run it. Dependencies are bundled. |
| Ubuntu / Debian | `.deb` | Install the downloaded file with `sudo apt install ./usetracker_*.deb`. |
| Fedora | `.rpm` | Install the downloaded file with `sudo dnf install ./usetracker-*.rpm`. |
| macOS | `.dmg` | Open the disk image and drag useTracker into Applications. |
| Windows | `.zip` | Extract the archive, then run `useTracker.exe`. |

For example, to run a downloaded AppImage:

```bash
chmod +x useTracker-*.AppImage
./useTracker-*.AppImage
```

The macOS app is not notarised. Follow your macOS version's procedure for opening
an app from an identified source; verify that you downloaded it from the project
release page.

:::caution[Platform support]
Development and most testing happen on Linux. Windows and macOS have build and
packaging scripts, but receive less testing. Reports either way are useful — see
[contributing](/project/contributing/).
:::

## Dependencies

| Requirement | Version | Notes |
| --- | --- | --- |
| CMake | 3.21+ | |
| C++ compiler | C++17 | GCC 12+, Clang 15+, MSVC 2022 |
| OpenCV | 4.2+ or 5.x **with contrib modules** | required, see below |
| SDL2 | recent | window and input |
| OpenGL | — | the interface renders with OpenGL 3 |
| FFmpeg | libavformat, libavcodec, libswscale, libavutil | decoding and H.264 recording |

Dear ImGui, CLI11 and portable-file-dialogs are downloaded automatically during
configuration, so the first `cmake` run needs network access.

The contrib modules are not optional. The build requires `bgsegm` (GMG and GSOC
background subtractors), `xphoto` (white balance), `tracking` (the CSRT tracker)
and `dnn` (YOLO inference). If `find_package` fails naming a component, that
component is missing from your OpenCV.

## Linux

The helper scripts detect common distributions, install dependencies, and build
the release binary:

```bash
git clone https://github.com/AlexandreCampo/useTracker.git
cd useTracker
./scripts/install-deps.sh
./scripts/build.sh
./build/useTracker
```

For a manual build, install the dependencies yourself:

```bash
# Arch / Manjaro
sudo pacman -S cmake opencv sdl2 ffmpeg

# Ubuntu / Debian
sudo apt install cmake build-essential \
    libopencv-dev libopencv-contrib-dev \
    libsdl2-dev libgl1-mesa-dev \
    libavformat-dev libavcodec-dev libswscale-dev libavutil-dev
```

```bash
git clone https://github.com/AlexandreCampo/useTracker.git
cd useTracker
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

The binary is `build/useTracker`.

Some distributions split the contrib modules across packages, and a few ship
OpenCV without `bgsegm` or `xphoto`. Building OpenCV yourself with
`opencv_contrib` is the reliable fallback.

## Windows

From a checkout, use the PowerShell helper, which bootstraps vcpkg and builds the
application. The first build also compiles its dependencies and can take a while.

```powershell
.\packaging\windows\build-windows.ps1
```

## macOS

```bash
brew install cmake opencv sdl2 ffmpeg
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.ncpu)
```

Homebrew's `opencv` includes the contrib modules.

## Build options

| Option | Default | Effect |
| --- | --- | --- |
| `CMAKE_BUILD_TYPE` | *(empty)* | Set to `Release`. An unoptimised build is several times slower. |
| `USE_ARUCO_COLOR` | `ON` | Builds the bundled colour-marker library, enabling the Aruco Color plugin. |
| `USE_BLUETOOTH` | `OFF` | Links libbluetooth for the Remote Control plugin (Linux). Deprecated — see [known issues](/project/known-issues/). |

## Checking it works

```bash
./build/useTracker --help
./build/useTracker -p examples/fish_pattern_tracking.xml -i /path/to/video.mp4
```

If the window opens and the video plays, continue with
[first analysis](/start/first-analysis/).

## Optional: a YOLO model

Needed only for the Yolo Detector plugin. No model ships with useTracker.

```bash
pip install ultralytics
yolo export model=yolov8n.pt format=onnx
```
