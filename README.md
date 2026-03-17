# useTracker

Real-time tracking and image processing software with a graphical interface. It can open images, videos, and USB cameras and run plugins on them. Plugins include tracking, blob extraction, H264 recording, ArUco marker detection, and other classic tools (erosion, dilation, segmentation, background subtraction, ...).

## Building

### Prerequisites

- **CMake** 3.20+
- **C++17** compiler (GCC 12+, Clang 15+, MSVC 2022)
- **OpenCV 4.x** with contrib modules (bgsegm, objdetect)
- **SDL2**
- **FFmpeg** (libavformat, libavcodec, libswscale, libavutil)

The following are fetched automatically via CMake FetchContent:
- Dear ImGui (v1.91.8)
- CLI11 (v2.4.2)
- portable-file-dialogs

### Linux

```bash
# Install dependencies (Arch/Manjaro)
sudo pacman -S cmake opencv sdl2 ffmpeg

# Install dependencies (Ubuntu/Debian)
sudo apt install cmake libopencv-dev libopencv-contrib-dev libsdl2-dev libavformat-dev libavcodec-dev libswscale-dev libavutil-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Windows

```powershell
# Using vcpkg
vcpkg install opencv4[contrib] sdl2 ffmpeg
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### macOS

```bash
brew install cmake opencv sdl2 ffmpeg
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `USE_BLUETOOTH` | OFF | Enable Bluetooth remote control (Linux only, requires libbluetooth) |
| `USE_ARUCO_COLOR` | ON | Build the custom ArucoColor marker library |

## Usage

### GUI mode
```bash
./useTracker
```

### Headless mode
```bash
./useTracker --nogui -i video.mp4 -x pipeline.xml
./useTracker --nogui --usb 0 -x pipeline.xml
```

Run `./useTracker --help` for all command-line options.

## License

GNU General Public License v3.0 - see [LICENSE](LICENSE) for details.
