# useTracker Migration Plan: Cross-Platform Modernization

## Context

useTracker is a scientific real-time image processing and blob tracking application (~5,500 lines core + ~2,000 plugin lines). Last updated 2022, it only builds on Linux, uses deprecated APIs (FFmpeg, OpenCV 2.x/3.x, ArUco 1.3.0), a dead build tool (cbp2make/premake4), and wxWidgets with wxsmith for UI. The goal is to make it compile and run on Linux/Windows/macOS with modern toolchains (2026), while preserving the well-designed plugin architecture and threading model.

**User decisions:** Drop Vimba camera support. Keep Bluetooth optional. Keep direct FFmpeg (modernize API). Full migration at once.

---

## Phase 0: CMake Build System

Create `CMakeLists.txt` with:
- C++17, `find_package` for OpenCV 4, SDL2, OpenGL, FFmpeg
- `FetchContent` for Dear ImGui (v1.90+), portable-file-dialogs, CLI11
- `option(USE_BLUETOOTH)` for optional libbluetooth
- `option(USE_ARUCO_COLOR)` with `add_subdirectory(arucoColor)`

Create `cmake/FindFFmpeg.cmake` for locating libavformat/libavcodec/libswscale/libavutil.

Create `vcpkg.json` manifest for cross-platform dependency resolution.

**Files to create:** `CMakeLists.txt`, `cmake/FindFFmpeg.cmake`, `vcpkg.json`

---

## Phase 1: Decouple Core from wxWidgets & Boost

Remove all wx/Boost dependencies from non-GUI code so the engine layer is pure C++17 + OpenCV + FFmpeg.

### 1.1 Replace `wxLongLong` timing with `std::chrono`
- `Capture.h` — remove `#include <wx/time.h>`, add `GetUTCTimeUSec()` helper using `std::chrono::steady_clock`
- All 7 capture files (`CaptureVideo`, `CaptureUSBCamera`, `CaptureImage`, `CaptureDefault`, `CaptureMultiVideo`, `CaptureMultiUSBCamera`) — change `wxLongLong` members to `int64_t`, replace `wxGetUTCTimeUSec()` calls

### 1.2 Replace `boost::program_options` with CLI11
- `Parameters.h/cpp` — rewrite `parseCommandLine()` using `CLI::App`
- Replace `boost::filesystem` with `std::filesystem` everywhere

### 1.3 Replace `wxFileName` in App.cpp
- Use `std::filesystem::path` for extension checking

### 1.4 Delete Vimba support
- **Delete:** `capture/CaptureAVTCamera.h/cpp`, entire `vimba/` directory, `MakefileVimba`
- **Modify:** Remove `#ifdef VIMBA` blocks from `App.cpp`, `ImageProcessingEngine.cpp`, `Capture.h`

**Files modified:** `Capture.h`, all `capture/*.cpp/h`, `Parameters.h/cpp`, `App.cpp`, `ImageProcessingEngine.cpp`
**Files deleted:** `capture/CaptureAVTCamera.h/cpp`, `vimba/*` (8 files), `MakefileVimba`

---

## Phase 2: Modernize FFmpeg API

Update deprecated FFmpeg C API calls to FFmpeg 5.x/6.x compatible code.

### 2.1 CaptureVideo (decode path) — `capture/CaptureVideo.h/cpp`
- Remove `av_register_all()`, `av_init_packet()`
- Use `avcodec_alloc_context3()` + `avcodec_parameters_to_context()` instead of `stream->codec`
- Replace `avcodec_decode_video2()` with `avcodec_send_packet()` / `avcodec_receive_frame()`
- Replace `avpicture_fill()` with `av_image_fill_arrays()`
- Use `av_packet_alloc()` / `av_packet_unref()` instead of stack `AVPacket`
- `AVCodec*` → `const AVCodec*`, `avframe->pkt_pts` → `avframe->pts`
- Remove version branching (`#if LIBAVCODEC_VERSION_INT`), always use modern API

### 2.2 CaptureMultiVideo — same decode path changes

### 2.3 RecordVideo (encode path) — `plugins/RecordVideo.h/cpp`
- Replace `avcodec_encode_video2()` with `avcodec_send_frame()` / `avcodec_receive_packet()`
- `CODEC_FLAG_GLOBAL_HEADER` → `AV_CODEC_FLAG_GLOBAL_HEADER`
- Replace `avpicture_get_size()` → `av_image_get_buffer_size()`
- `AVCodec*` → `const AVCodec*`, `AVOutputFormat*` → `const AVOutputFormat*`

**Files modified:** `capture/CaptureVideo.h/cpp`, `capture/CaptureMultiVideo.h/cpp`, `plugins/RecordVideo.h/cpp`

---

## Phase 3: OpenCV 4.x API Updates

### 3.1 Remove OpenCV 2.x compatibility
- `plugins/BackgroundDiffMOG.h/cpp` — drop `#if CV_MAJOR_VERSION == 2` branches, keep only `cv::bgsegm::createBackgroundSubtractorMOG()`
- `plugins/BackgroundDiffMOG2.h/cpp` — keep `cv::createBackgroundSubtractorMOG2()` only
- `plugins/BackgroundDiffGMG.h/cpp` — keep `cv::bgsegm::createBackgroundSubtractorGMG()` only
- Remove `CV_MAJOR_VERSION` checks in `CaptureMultiVideo.cpp`, `CaptureMultiUSBCamera.cpp`, `CaptureDefault.cpp`

### 3.2 Update deprecated constants (all files)
- `CV_LOAD_IMAGE_GRAYSCALE` → `cv::IMREAD_GRAYSCALE`
- `CV_CAP_PROP_*` → `cv::CAP_PROP_*`
- `CV_FILLED` → `cv::FILLED`
- `CV_IMWRITE_PNG_COMPRESSION` → `cv::IMWRITE_PNG_COMPRESSION`
- Modernize include paths: `<opencv2/core/core.hpp>` → `<opencv2/core.hpp>`

### 3.3 Migrate ArUco plugin — `plugins/Aruco.h/cpp` (complete rewrite)
- Remove `#include <aruco/aruco.h>` (standalone 1.3.0)
- Use `#include <opencv2/objdetect/aruco_detector.hpp>` (OpenCV 4.x built-in)
- Replace `aruco::MarkerDetector` → `cv::aruco::ArucoDetector`
- Replace `aruco::Marker` → `std::vector<std::vector<cv::Point2f>> corners` + `std::vector<int> ids`
- Map threshold parameters to `cv::aruco::DetectorParameters`

### 3.4 Audit arucoColor/ library
- Update any deprecated OpenCV calls (`CV_FILLED`, old include paths)
- Convert `premake4.lua` build to CMake `add_library(arucoColor STATIC ...)`

**Files modified:** All `BackgroundDiff*.h/cpp`, `Aruco.h/cpp`, `ExtractBlobs.cpp`, `GetBlobsAngles.cpp`, `ImageProcessingEngine.cpp`, `CaptureUSBCamera.cpp`, `arucoColor/*.cpp`

---

## Phase 4: SDL2 + Dear ImGui Application Shell

### 4.1 Rewrite entry point — `App.h/cpp`
- Remove `wxApp` inheritance, create plain `main()`
- Keep plugin factory registration (`NewPipelinePluginVector` map) as-is
- Route to headless engine loop (nogui) or new `AppGui` class

### 4.2 Create AppGui — **new files: `AppGui.h/cpp`**
```cpp
class AppGui {
    SDL_Window* window;
    SDL_GLContext gl_context;
    GLuint videoTexture;
    ImageProcessingEngine engine;
    // UI state...
public:
    int Run();  // SDL event loop + ImGui render loop
};
```
- `InitSDL()` — create window, GL context
- `InitImGui()` — ImGui context, SDL2+OpenGL3 backends, load icon textures
- Main loop: `SDL_PollEvent` → `UpdateEngine()` → `RenderFrame()` → `SDL_GL_SwapWindow`
- `UpdateEngine()` — port `MainFrame::OnIdle()` logic (frame timing, pipeline stepping)
- Upload `cv::Mat` to GL texture via `glTexImage2D`, display with `ImGui::Image()`

### 4.3 Icon resources
- Load PNG icons from `images/` directory using OpenCV or stb_image
- Upload as GL textures for toolbar buttons
- Use relative path resolution (executable-relative or config-based)

**Files created:** `AppGui.h`, `AppGui.cpp`
**Files modified:** `App.h`, `App.cpp`

---

## Phase 5: Main Window UI (ImGui)

Recreate all `MainFrame.cpp` functionality (~3,067 lines) as ImGui rendering code in `AppGui.cpp`.

### 5.1 Menu bar — `ImGui::BeginMainMenuBar()`
- File: Open Source, Save Source, Load Settings, Save Settings, Quit
- Help: About
- File dialogs via `portable-file-dialogs` (native OS dialogs)

### 5.2 Toolbar — horizontal `ImGui::ImageButton()` row
- Record, Stop, Rewind, StepBack, Play/Pause, StepForward, Forward
- Video position slider — `ImGui::SliderFloat()`
- HUD toggle button
- Processing blending slider

### 5.3 Four-tab notebook — `ImGui::BeginTabBar()`
- **Processing tab:** Plugin list with checkboxes (`ImGui::Checkbox` + `ImGui::Selectable`), add/remove/up/down buttons, available plugins list
- **Background tab:** Method radio buttons, spin controls, load/save/recalculate buttons, zones file picker
- **Calibration tab:** Board type radio, dimension inputs, aspect ratio, calibration buttons, stitching controls
- **Processing Frame tab:** Start time, duration, timestep inputs, time bounds checkbox

### 5.4 Video display — central `ImGui::Image()` with GL texture
- Zoom: left-click drag marquee → compute new UV coordinates
- Pan: right-click drag → shift UV offset
- Mouse wheel zoom
- Coordinate display on hover

### 5.5 Keyboard shortcuts
- Map existing char event handlers to `ImGui::IsKeyPressed()` checks

**Files modified:** `AppGui.cpp` (bulk of the work)
**Files deleted:** `MainFrame.h`, `MainFrame.cpp`

---

## Phase 6: Dialog Migration (21 Dialogs → ImGui Panels)

Each wxDialog becomes an ImGui window drawing function. Widget mapping:

| wxWidgets | ImGui |
|---|---|
| wxSpinCtrl | `ImGui::InputInt` / `ImGui::DragInt` |
| wxSpinCtrlDouble | `ImGui::InputFloat` / `ImGui::DragFloat` |
| wxCheckBox | `ImGui::Checkbox` |
| wxRadioBox | `ImGui::RadioButton` |
| wxSlider | `ImGui::SliderInt` / `ImGui::SliderFloat` |
| wxFilePickerCtrl | `ImGui::InputText` + Browse button (portable-file-dialogs) |
| wxColourPickerCtrl | `ImGui::ColorEdit3` |
| wxListView | `ImGui::BeginTable` |
| wxTextCtrl | `ImGui::InputText` |
| wxChoice | `ImGui::Combo` |

Create new dialog files (one per plugin or consolidated) replacing the 21 wx dialog `.h/.cpp` pairs.

Most complex dialog: `DialogOpenCapture` (7 source type tabs) — implement as `ImGui::BeginTabBar` with per-source-type panels.

**Files created:** New ImGui dialog files (in `dialogs/` or `ui/`)
**Files deleted:** All 21 `dialogs/Dialog*.h/cpp` pairs, all 16 `wxsmith/*.wxs` files

---

## Phase 7: Cleanup and Cross-Platform Polish

### 7.1 Delete legacy files
- `Makefile`, `useTracker.cbp`, `arucoColor/premake4.lua`
- Old setup/update scripts (or update them)

### 7.2 Resource path handling
- Replace hardcoded `/usr/share/useTracker/images/` with executable-relative path resolution
- CMake install rules for all platforms

### 7.3 Cross-platform build verification
- Linux: GCC 12+ / Clang 15+
- Windows: MSVC 2022 + vcpkg
- macOS: AppleClang + Homebrew, OpenGL 3.2+ context

### 7.4 Update README.md
- New build instructions for all three platforms
- Document CMake options
- Document vcpkg setup

---

## Verification Plan

After each phase, verify with:
1. **Phase 0-1:** `cmake --build .` succeeds; headless mode runs: `./useTracker --nogui -i test.mp4`
2. **Phase 2:** Video decode/encode works end-to-end in nogui mode, seek is accurate
3. **Phase 3:** All plugins compile against OpenCV 4.x; ArUco detection works with test markers
4. **Phase 4:** SDL2 window appears with ImGui demo + static test image
5. **Phase 5:** Full GUI works — open video, play/pause/seek, pipeline runs, tabs functional
6. **Phase 6:** All plugin dialogs open and modify parameters in real-time
7. **Phase 7:** Clean build on Linux, Windows, macOS; full workflow test

## Risk Areas

| Risk | Severity | Mitigation |
|---|---|---|
| FFmpeg send/receive API (async semantics differ from old sync API) | High | Test seeking thoroughly with known videos |
| ArUco 1.3.0 → OpenCV 4.x cv::aruco (different parameter model) | Medium | Map each parameter, test with known marker images |
| ArucoColor custom library (may use deprecated OpenCV internally) | Medium | Audit and update, or flag as optional |
| wxLongLong timing precision during migration | Low | Unit test frame timing accuracy |

## Estimated Scope

- ~30 files modified significantly
- ~8 files created (CMake, AppGui, new dialog files)
- ~50 files deleted (wxsmith, vimba, old dialogs, old build files)
- Core plugin logic (~2,000 lines) transfers with minimal changes
- MainFrame rewrite is the largest single effort (~3,000 lines wx → ~1,500 lines ImGui)
