# useTracker

Real-time tracking and image processing software with a graphical interface. It can open images, videos, and USB cameras and run plugins on them. Plugins include tracking, blob extraction, H264 recording, ArUco marker detection, deep-learning object detection (YOLO), image enhancement (CLAHE, white balance), and other classic tools (erosion, dilation, segmentation, background subtraction, ...).

Pipelines are built by stacking plugins; each plugin refines a shared *marked* mask that downstream plugins (Extract Blobs, Track Blobs) consume. Enhancement plugins modify the frame in place so their effect is visible live and benefits the whole pipeline, while the original frame is preserved for recording.

### Background subtraction

Several background subtractors are available: **Background Difference** (static background image), **MOG**, **MOG2**, **GMG**, **KNN**, and **GSOC**. For difficult footage (e.g. turbid underwater scenes) KNN and GSOC on the raw frames tend to give the cleanest foreground masks. When paused, the model history stays frozen — it only advances on real movie frames, never on repeated views of a static frame.

### Pattern tracking

The **Pattern Tracker** plugin follows a target by its appearance. Once a target is seeded — by clicking on the video, or automatically from a detected blob (place the plugin after **Extract Blobs**) — it stores the image patch around it and, each following frame, searches a limited neighbourhood (*search distance*) for the best match, stepping the target to the peak. Two backends are available:

- **Template match** — explicit local-window normalized cross-correlation, with a template that adapts only when the match is confident (avoids drifting onto the background). Every parameter (search distance, match/update thresholds, update rate) is exposed.
- **CSRT** — OpenCV's discriminative correlation-filter tracker, more robust to appearance and scale change.

Multiple targets are tracked at once. A short-term motion prediction centres the search on the expected position, and targets lost for too long (or that leave the frame) are dropped. Tracks are drawn on the HUD with a trail, written to CSV, and stamped into the *marked* mask so **Track Blobs** can consume them. All settings are saved in the settings file.

### Deep-learning detection (YOLO)

The **Yolo Detector** plugin runs any ONNX YOLO model (v5 / v8 / v11 layouts are auto-detected) through OpenCV's DNN module on the CPU. Provide:

- a model file (`.onnx`) — no model ships with useTracker; export one with e.g. `yolo export model=yolov8n.pt format=onnx` (Ultralytics)
- an optional class-names file (one name per line)

Set the confidence / NMS thresholds and an optional class filter (comma-separated names or ids). Detections are drawn on the HUD, written to a CSV file, and stamped into the *marked* mask so Extract Blobs / Track Blobs can consume them. The model path is saved in the settings file and reloads automatically when the settings are reopened. CPU inference runs at a few fps at 640×640 — suited to offline analysis rather than live 25 fps capture.

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
