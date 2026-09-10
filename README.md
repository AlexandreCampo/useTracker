# useTracker

Real-time tracking and image processing software with a graphical interface. It can open images, videos, and USB cameras and run plugins on them. Plugins include tracking, blob extraction, H264 recording, ArUco marker detection, deep-learning object detection (YOLO), image enhancement (CLAHE, white balance), and other classic tools (erosion, dilation, segmentation, background subtraction, ...).

Pipelines are built by stacking plugins; each plugin refines a shared *marked* mask that downstream plugins (Extract Blobs, Track Blobs) consume. Enhancement plugins modify the frame in place so their effect is visible live and benefits the whole pipeline, while the original frame is preserved for recording.

### Image enhancement

Several plugins improve the image before detection (all applied in place):

- **White Balance** — automatic (gray-world / simple), **manual** with per-channel gains you can set by clicking a should-be-neutral pixel (*pick white*), or **underwater** mode which restores the red channel absorbed by water with a single strength slider. The underwater mode removes the blue-green cast far better than generic auto white balance.
- **Clahe** — local contrast enhancement (CLAHE on the lightness channel).
- **Curves** — general per-channel tone/colour curves. Each of the Master, Red, Green and Blue channels has an editable curve (drag points on the graph; click to add, right-click to remove), interpolated with a monotone cubic spline. Curves subsume levels, gamma, contrast and colour balance in one tool.

### Regions of interest (ROI)

Add a **Zones of Interest** plugin to the pipeline and configure it entirely in its dialog. Zones are defined by **drawing polygons directly on the video** and/or from a **zone-mask image** (grayscale, pixel value = region number). Select the plugin in the pipeline to edit on the video: click to add polygon vertices, drag vertices to move them, click the first vertex (the cursor changes) or right-click to close a polygon, and right-click a vertex to delete it. Each polygon has a **region number** (0, 1, 2, …); region 0 and un-painted areas are ignored by downstream plugins, and a plugin's *Restrict to Zone* option keeps only the pixels of a chosen region. Polygons are saved to / loaded from a plain text file, and are also embedded in the settings file so a configuration is self-contained.

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

Set the confidence / NMS thresholds and an optional class filter (comma-separated names or ids). Detections are drawn on the HUD, written to a CSV file, and stamped into the *marked* mask so Extract Blobs / Track Blobs can consume them. The model path is saved in the settings file and reloads automatically when the settings are reopened.

A **Compute Target** can be selected (CPU / OpenCL / OpenCL FP16 / Vulkan). The GPU targets are used only if a working OpenCL or Vulkan runtime is present and can run the model — the plugin probes the target on load and silently falls back to the CPU otherwise. On an integrated GPU expect a modest speed-up over the multi-threaded CPU path; CPU inference runs at a few fps at 640×640, suited to offline analysis rather than live 25 fps capture. NVIDIA CUDA is only available if OpenCV itself was built with CUDA.

## Installing

Prebuilt binaries for every release are on the
[releases page](https://github.com/AlexandreCampo/useTracker/releases).

| Your system | Download | How to run it |
|---|---|---|
| Any Linux — Ubuntu, Manjaro, Arch, Fedora, Debian… | `useTracker-*.AppImage` | `chmod +x useTracker-*.AppImage && ./useTracker-*.AppImage` |
| Ubuntu / Debian | `usetracker_*_amd64.deb` | `sudo apt install ./usetracker_*_amd64.deb` |
| Fedora / openSUSE | `usetracker-*.rpm` | `sudo dnf install ./usetracker-*.rpm` |
| Arch / Manjaro | build it natively | `cd packaging/arch && makepkg -si` |
| macOS | `useTracker-*-Darwin-*.dmg` | open the disk image, drag the app to Applications |
| Windows | `useTracker-*-Windows-x64.zip` | unzip anywhere, run `useTracker.exe` |

The AppImage is self-contained: it carries its own OpenCV, FFmpeg and SDL2, so
it keeps working after a system upgrade changes those libraries — useful on
rolling-release distributions.

On macOS the app is not notarised; on first launch, right-click it and choose
*Open* (or run `xattr -dr com.apple.quarantine /Applications/useTracker.app`).

## Building

### Prerequisites

- **CMake** 3.21+
- **C++17** compiler (GCC 12+, Clang 15+, MSVC 2022)
- **OpenCV** 4.2+ or 5.x, with contrib modules (bgsegm, xphoto, tracking)
- **SDL2**
- **FFmpeg** 4.4+ (libavformat, libavcodec, libswscale, libavutil)

Tested from Ubuntu 22.04 (OpenCV 4.5, FFmpeg 4.4) to current Arch/Manjaro
(OpenCV 5.0, FFmpeg 9).

Dear ImGui, CLI11 and portable-file-dialogs are fetched automatically by CMake
and need no system package.

### Linux and macOS

```bash
./scripts/install-deps.sh     # apt, pacman, dnf, zypper or Homebrew — detected
./scripts/build.sh            # release build into build/
./build/useTracker
```

`scripts/build.sh` takes `--type Debug`, `--jobs N`, `--prefix DIR`, `--install`,
`--package`, `--bluetooth` and `--clean`; `--help` lists them all.

### Windows

```powershell
.\packaging\windows\build-windows.ps1
```

Dependencies come from vcpkg, which the script bootstraps if `VCPKG_ROOT` is not
already set. The first build compiles OpenCV from source and takes about an hour.

### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `USE_BLUETOOTH` | OFF | Enable Bluetooth remote control (Linux only, requires libbluetooth) |
| `USE_ARUCO_COLOR` | ON | Build the custom ArucoColor marker library |
| `USETRACKER_BUNDLE_DEPS` | ON for macOS/Windows | Copy third-party libraries into the install tree |
| `USETRACKER_MACOS_SIGN_IDENTITY` | `-` (ad-hoc) | codesign identity for the macOS bundle |

### Building the distributable packages

```bash
./scripts/build.sh --package              # .tar.gz, .deb, .rpm — for this distribution
./packaging/linux/build-appimage.sh       # one binary that runs on any distribution
./packaging/linux/build-in-docker.sh      # the same, in a container — needs only Docker
./packaging/macos/build-macos.sh          # useTracker.app in a .dmg
```

Artifacts land in `dist/`. See [packaging/README.md](packaging/README.md) for the
details, including which distribution to build on and how releases are cut.

## Usage

### GUI mode
```bash
./useTracker
```

The desktop workspace keeps the image on the left and the analysis pipeline on
the right. Open a video or image with **Open…**, `Ctrl+O`, or drag and drop. Search
the plugin library, add stages, then double-click a stage to edit its parameters.
Drag stages to reorder them; selecting a stage chooses its mask and overlays
for preview. **Image / Blend / Mask** switches the display, with measurements
and blend strength under **Tools**.

Playback and timeline markers sit below the image. **Write outputs** enables the
outputs configured in active plugins. **Save…** saves the analysis as XML. The
bottom bar controls processing resolution and interface size; both panel dividers
can be dragged. Interface size and divider positions are saved in `imgui.ini`.
The **Help → Keyboard & mouse** panel lists the shortcuts.

Use the **Light mode / Dark mode** button at the top right (or **View**) to switch
themes. Dark mode pairs charcoal with amber; Light mode uses warm off-white,
graphite text and brass accents. The choice is saved with the workspace and also
applies to parameter dialogs, menus and plots.

GUI regression checks are described in [tests/gui/README.md](tests/gui/README.md).

### Headless mode
```bash
./useTracker --nogui -i video.mp4 -p pipeline.xml
./useTracker --nogui --usb 0 -p pipeline.xml
```

Run `./useTracker --help` for all command-line options.

## License

GNU General Public License v3.0 - see [LICENSE](LICENSE) for details.

Bundled Liberation fonts use the [SIL Open Font License 1.1](ui/fonts/OFL.txt).
