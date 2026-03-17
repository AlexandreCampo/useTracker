cmake_minimum_required(VERSION 3.20)
project(useTracker VERSION 2.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(USE_BLUETOOTH   "Enable Bluetooth remote control (libbluetooth)" OFF)
option(USE_ARUCO_COLOR "Build the ArucoColor custom marker library"     ON)

# ---------------------------------------------------------------------------
# Dependencies — system / vcpkg
# ---------------------------------------------------------------------------
find_package(OpenCV 4 REQUIRED COMPONENTS
    core imgproc highgui videoio objdetect calib3d features2d bgsegm)
find_package(SDL2     REQUIRED)
find_package(OpenGL   REQUIRED)
find_package(FFmpeg   REQUIRED)   # cmake/FindFFmpeg.cmake

# ---------------------------------------------------------------------------
# Dependencies — FetchContent (header-only / small libs)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(portable_file_dialogs
    GIT_REPOSITORY https://github.com/samhocevar/portable-file-dialogs.git
    GIT_TAG        main
)
# Work around old cmake_minimum_required in portable-file-dialogs
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(portable_file_dialogs)
unset(CMAKE_POLICY_VERSION_MINIMUM)

FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.2
)
FetchContent_MakeAvailable(cli11)

# ---------------------------------------------------------------------------
# ImGui library (SDL2 + OpenGL3 backends)
# ---------------------------------------------------------------------------
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC
    SDL2::SDL2
    OpenGL::GL
)

# ---------------------------------------------------------------------------
# ArucoColor (optional static library)
# ---------------------------------------------------------------------------
if(USE_ARUCO_COLOR)
    add_library(arucoColor STATIC
        arucoColor/arucoColor.cpp
        arucoColor/dictionary.cpp
        arucoColor/generateMarkers.cpp
        arucoColor/marker.cpp
        arucoColor/printMarkers.cpp
    )
    target_include_directories(arucoColor PUBLIC
        ${CMAKE_SOURCE_DIR}/arucoColor
    )
    target_link_libraries(arucoColor PUBLIC ${OpenCV_LIBS})
endif()

# ---------------------------------------------------------------------------
# Core engine sources (no GUI dependency)
# ---------------------------------------------------------------------------
set(CORE_SOURCES
    Capture.cpp
    CaptureCalibration.cpp
    ImageProcessingEngine.cpp
    Pipeline.cpp
    Parameters.cpp
    Background.cpp
    Utils.cpp
    Blob.h

    capture/CaptureDefault.cpp
    capture/CaptureImage.cpp
    capture/CaptureUSBCamera.cpp
    capture/CaptureVideo.cpp
    capture/CaptureMultiVideo.cpp
    capture/CaptureMultiUSBCamera.cpp

    plugins/AdaptiveThreshold.cpp
    plugins/Aruco.cpp
    plugins/ArucoColor.cpp
    plugins/BackgroundDiffGMG.cpp
    plugins/BackgroundDiffMOG.cpp
    plugins/BackgroundDiffMOG2.cpp
    plugins/ColorSegmentation.cpp
    plugins/Dilation.cpp
    plugins/Erosion.cpp
    plugins/ExtractBlobs.cpp
    plugins/ExtractMotion.cpp
    plugins/FrameDifference.cpp
    plugins/GetBlobsAngles.cpp
    plugins/MovingAverage.cpp
    plugins/RecordPixels.cpp
    plugins/RecordVideo.cpp
    plugins/RemoteControl.cpp
    plugins/SafeErosion.cpp
    plugins/SimpleTags.cpp
    plugins/Stopwatch.cpp
    plugins/TakeSnapshots.cpp
    plugins/Tracker.cpp
    plugins/ZonesOfInterest.cpp
)

# ---------------------------------------------------------------------------
# GUI sources
# ---------------------------------------------------------------------------
set(GUI_SOURCES
    App.cpp
    AppGui.cpp
)

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
add_executable(useTracker ${CORE_SOURCES} ${GUI_SOURCES})

target_include_directories(useTracker PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/capture
    ${CMAKE_SOURCE_DIR}/plugins
    ${FFmpeg_INCLUDE_DIRS}
)

target_link_libraries(useTracker PRIVATE
    ${OpenCV_LIBS}
    FFmpeg::avformat
    FFmpeg::avcodec
    FFmpeg::swscale
    FFmpeg::avutil
    imgui_lib
    SDL2::SDL2
    OpenGL::GL
    CLI11::CLI11
    portable_file_dialogs
)

if(USE_ARUCO_COLOR)
    target_link_libraries(useTracker PRIVATE arucoColor)
    target_compile_definitions(useTracker PRIVATE USE_ARUCO_COLOR)
endif()

if(USE_BLUETOOTH)
    find_library(BLUETOOTH_LIB bluetooth)
    if(BLUETOOTH_LIB)
        target_link_libraries(useTracker PRIVATE ${BLUETOOTH_LIB})
        target_compile_definitions(useTracker PRIVATE USE_BLUETOOTH)
    else()
        message(WARNING "libbluetooth not found — building without Bluetooth support")
    endif()
endif()

# Threads
find_package(Threads REQUIRED)
target_link_libraries(useTracker PRIVATE Threads::Threads)

# Platform-specific
if(UNIX AND NOT APPLE)
    target_link_libraries(useTracker PRIVATE ${CMAKE_DL_LIBS})
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
install(TARGETS useTracker RUNTIME DESTINATION bin)
install(DIRECTORY images/ DESTINATION share/useTracker/images)
