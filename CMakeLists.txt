cmake_minimum_required(VERSION 3.21)
project(useTracker
    VERSION      2.0.0
    DESCRIPTION  "Real-time image processing and blob tracking for scientific experiments"
    HOMEPAGE_URL "https://github.com/AlexandreCampo/useTracker"
    LANGUAGES    CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Release unless told otherwise — an unoptimised tracker is unusable in real time.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

# The macOS bundle carries its dylibs in Contents/Frameworks and needs an rpath
# that survives being moved to another machine. On Linux the system search path
# is correct for distribution packages, and the AppImage gets its rpath patched
# by linuxdeploy — so nothing to set there.
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
endif()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(USE_BLUETOOTH   "Enable Bluetooth remote control (libbluetooth)" OFF)
option(USE_ARUCO_COLOR "Build the ArucoColor custom marker library"     ON)
if(APPLE OR WIN32)
    set(_bundle_deps_default ON)   # DMG / ZIP must run without Homebrew or vcpkg
else()
    set(_bundle_deps_default OFF)  # distro packages link against system libraries
endif()
option(USETRACKER_BUNDLE_DEPS
    "Copy third-party runtime libraries into the install tree (macOS/Windows)"
    ${_bundle_deps_default})

# ---------------------------------------------------------------------------
# Dependencies — system / vcpkg / Homebrew
# ---------------------------------------------------------------------------
# OpenCV 4.2+ or 5.x, with the contrib modules (bgsegm, xphoto, tracking).
# 5.x renamed calib3d -> calib and features2d -> features, so the component list
# is picked after a minimal probe for the version. OpenCVConfig.cmake never
# clears OpenCV_LIBS (it accumulates across calls, by design), hence the probe
# asks for `core` only and OpenCV_LIBS is reset before the real request —
# otherwise every installed module ends up on the link line, including ones the
# distribution may have left in a broken state.
find_package(OpenCV REQUIRED COMPONENTS core)
if(OpenCV_VERSION VERSION_LESS 4.2)
    message(FATAL_ERROR "useTracker requires OpenCV >= 4.2 (found ${OpenCV_VERSION})")
endif()

set(_opencv_components
    core imgproc highgui imgcodecs videoio objdetect video photo dnn
    bgsegm xphoto tracking)
if(OpenCV_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND _opencv_components calib features geometry)
else()
    list(APPEND _opencv_components calib3d features2d)
    # aruco is a separate contrib module before 4.7; 4.7+ merged it into objdetect
    if(OpenCV_VERSION VERSION_LESS 4.7)
        list(APPEND _opencv_components aruco)
    endif()
endif()
unset(OpenCV_LIBS)
find_package(OpenCV REQUIRED COMPONENTS ${_opencv_components})
find_package(OpenGL   REQUIRED)
find_package(FFmpeg   REQUIRED)   # cmake/FindFFmpeg.cmake
find_package(Threads  REQUIRED)

# SDL2 config files differ between distributions, Homebrew and vcpkg: some
# export the SDL2::SDL2 target, older ones only set SDL2_LIBRARIES. Normalise
# on SDL2::SDL2, falling back to pkg-config.
find_package(SDL2 QUIET)
if(NOT TARGET SDL2::SDL2)
    if(SDL2_FOUND AND SDL2_LIBRARIES)
        add_library(SDL2::SDL2 INTERFACE IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
            INTERFACE_LINK_LIBRARIES      "${SDL2_LIBRARIES}"
        )
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
        add_library(SDL2::SDL2 ALIAS PkgConfig::SDL2)
    endif()
endif()

# ---------------------------------------------------------------------------
# Dependencies — FetchContent (header-only / small libs)
# ---------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(portable_file_dialogs
    GIT_REPOSITORY https://github.com/samhocevar/portable-file-dialogs.git
    GIT_TAG        main   # header-only, no tagged release carries the fixes we need
)
# Work around old cmake_minimum_required in portable-file-dialogs
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(portable_file_dialogs)
unset(CMAKE_POLICY_VERSION_MINIMUM)

FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.2
    GIT_SHALLOW    TRUE
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
    plugins/BackgroundDiffGSOC.cpp
    plugins/BackgroundDiffKNN.cpp
    plugins/BackgroundDiffMOG.cpp
    plugins/BackgroundDiffMOG2.cpp
    plugins/Clahe.cpp
    plugins/ColorSegmentation.cpp
    plugins/Curves.cpp
    plugins/Dehaze.cpp
    plugins/Denoise.cpp
    plugins/Dilation.cpp
    plugins/Erosion.cpp
    plugins/ExtractBlobs.cpp
    plugins/ExtractMotion.cpp
    plugins/FrameDifference.cpp
    plugins/GetBlobsAngles.cpp
    plugins/MovingAverage.cpp
    plugins/PatternTracker.cpp
    plugins/RecordPixels.cpp
    plugins/RecordVideo.cpp
    plugins/RemoteControl.cpp
    plugins/SafeErosion.cpp
    plugins/Sharpen.cpp
    plugins/SimpleTags.cpp
    plugins/Stopwatch.cpp
    plugins/TakeSnapshots.cpp
    plugins/TemporalDenoise.cpp
    plugins/Tracker.cpp
    plugins/WhiteBalance.cpp
    plugins/YoloDetector.cpp
    plugins/ZonesOfInterest.cpp
)

# ---------------------------------------------------------------------------
# GUI sources
# ---------------------------------------------------------------------------
include(cmake/EmbedFonts.cmake)
set(GUI_SOURCES
    App.cpp
    AppGui.cpp
    ui/Darkroom.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/DarkroomFonts.cpp"
)

# ---------------------------------------------------------------------------
# Platform resources (icon for the macOS bundle / Windows executable)
# ---------------------------------------------------------------------------
set(PLATFORM_SOURCES "")
set(MACOSX_ICON "${CMAKE_SOURCE_DIR}/packaging/icons/useTracker.icns")

if(APPLE AND EXISTS "${MACOSX_ICON}")
    list(APPEND PLATFORM_SOURCES "${MACOSX_ICON}")
    set_source_files_properties("${MACOSX_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
elseif(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/packaging/icons/useTracker.ico")
    # The .rc is generated so the icon is referenced by absolute path.
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/packaging/icons/useTracker.ico" USETRACKER_ICO)
    string(REPLACE "\\" "\\\\" USETRACKER_ICO "${USETRACKER_ICO}")
    configure_file(
        "${CMAKE_SOURCE_DIR}/packaging/windows/useTracker.rc.in"
        "${CMAKE_BINARY_DIR}/useTracker.rc" @ONLY)
    list(APPEND PLATFORM_SOURCES "${CMAKE_BINARY_DIR}/useTracker.rc")
endif()

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------
# MACOSX_BUNDLE produces useTracker.app; it is ignored on other platforms.
# No WIN32 keyword: the console subsystem keeps `--nogui` output usable on Windows.
add_executable(useTracker MACOSX_BUNDLE
    ${CORE_SOURCES} ${GUI_SOURCES} ${PLATFORM_SOURCES})

target_include_directories(useTracker PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/capture
    ${CMAKE_SOURCE_DIR}/plugins
    ${FFmpeg_INCLUDE_DIRS}
)

target_compile_definitions(useTracker PRIVATE
    USETRACKER_VERSION="${PROJECT_VERSION}")

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
    Threads::Threads
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

if(UNIX AND NOT APPLE)
    target_link_libraries(useTracker PRIVATE ${CMAKE_DL_LIBS})
endif()

if(MSVC)
    target_compile_options(useTracker PRIVATE /bigobj /utf-8)
    target_compile_definitions(useTracker PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

if(APPLE)
    set_target_properties(useTracker PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST     "${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME    "useTracker"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.usetracker.useTracker"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT      "GPL-3.0-or-later, Alexandre Campo"
        MACOSX_BUNDLE_ICON_FILE      "useTracker.icns"
    )
endif()

# On Windows the DLLs must sit next to the .exe to run from the build tree.
if(WIN32)
    add_custom_command(TARGET useTracker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:useTracker> $<TARGET_FILE_DIR:useTracker>
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
include(GNUInstallDirs)

if(APPLE OR WIN32)
    # Self-contained layout: the app (or .exe + DLLs) sits at the root of the
    # DMG / ZIP, with data next to it.
    set(USETRACKER_BINDIR  ".")
    set(USETRACKER_DATADIR ".")
    set(USETRACKER_DOCDIR  ".")
else()
    set(USETRACKER_BINDIR  "${CMAKE_INSTALL_BINDIR}")
    set(USETRACKER_DATADIR "${CMAKE_INSTALL_DATADIR}/useTracker")
    set(USETRACKER_DOCDIR  "${CMAKE_INSTALL_DOCDIR}")
endif()

install(TARGETS useTracker
    RUNTIME DESTINATION "${USETRACKER_BINDIR}"
    BUNDLE  DESTINATION "."
)

if(APPLE)
    # Inside a bundle, data belongs in Resources.
    install(DIRECTORY examples/ DESTINATION "useTracker.app/Contents/Resources/examples")
    install(DIRECTORY images/   DESTINATION "useTracker.app/Contents/Resources/images")
    install(FILES README.md LICENSE DESTINATION ".")
    install(FILES ui/fonts/OFL.txt DESTINATION "useTracker.app/Contents/Resources/licenses/Liberation")
else()
    install(DIRECTORY examples/ DESTINATION "${USETRACKER_DATADIR}/examples")
    install(DIRECTORY images/   DESTINATION "${USETRACKER_DATADIR}/images")
    install(FILES README.md LICENSE DESTINATION "${USETRACKER_DOCDIR}")
    install(FILES ui/fonts/OFL.txt DESTINATION "${USETRACKER_DOCDIR}/licenses/Liberation")
endif()

if(WIN32)
    # Third-party DLLs resolved from the imported targets (vcpkg, mingw, ...).
    install(FILES $<TARGET_RUNTIME_DLLS:useTracker>
        DESTINATION "${USETRACKER_BINDIR}")
endif()

if(UNIX AND NOT APPLE)
    install(FILES packaging/linux/useTracker.desktop
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(FILES packaging/linux/org.usetracker.useTracker.metainfo.xml
        DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo")
    install(FILES packaging/icons/useTracker.svg
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps")
    foreach(_size 64 128 256)
        install(FILES "packaging/icons/useTracker-${_size}.png"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_size}x${_size}/apps"
            RENAME useTracker.png)
    endforeach()
endif()

# ---------------------------------------------------------------------------
# Bundle third-party libraries into the macOS .app so it runs on a machine
# without Homebrew. Linux redistributables go through AppImage instead
# (packaging/linux/build-appimage.sh), which handles the exclude list properly.
# ---------------------------------------------------------------------------
if(APPLE AND USETRACKER_BUNDLE_DEPS)
    set(_fixup_dirs "${FFmpeg_LIBRARY_DIRS}")
    if(OpenCV_INSTALL_PATH)
        list(APPEND _fixup_dirs "${OpenCV_INSTALL_PATH}/lib")
    endif()
    list(APPEND _fixup_dirs /opt/homebrew/lib /usr/local/lib)
    list(REMOVE_DUPLICATES _fixup_dirs)

    install(CODE "
        include(BundleUtilities)
        set(BU_CHMOD_BUNDLE_ITEMS TRUE)
        fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/useTracker.app\" \"\" \"${_fixup_dirs}\")
    ")

    # fixup_bundle rewrites the dylib load paths, which invalidates the
    # signature the linker applied. An unsigned binary refuses to launch on
    # Apple Silicon, so sign again — ad-hoc by default, or with a Developer ID
    # certificate when one is given.
    set(USETRACKER_MACOS_SIGN_IDENTITY "-" CACHE STRING
        "codesign identity for the bundle (\"-\" = ad-hoc, empty = do not sign)")
    if(USETRACKER_MACOS_SIGN_IDENTITY)
        install(CODE "
            message(STATUS \"Signing useTracker.app (${USETRACKER_MACOS_SIGN_IDENTITY})\")
            execute_process(COMMAND codesign --force --deep
                --sign \"${USETRACKER_MACOS_SIGN_IDENTITY}\"
                \"\${CMAKE_INSTALL_PREFIX}/useTracker.app\"
                RESULT_VARIABLE _sign_result)
            if(NOT _sign_result EQUAL 0)
                message(WARNING \"codesign failed — the bundle may refuse to launch\")
            endif()
        ")
    endif()
endif()

# ---------------------------------------------------------------------------
# Packaging (CPack): DEB/RPM/TGZ on Linux, DMG on macOS, ZIP/NSIS on Windows
# ---------------------------------------------------------------------------
include(cmake/Packaging.cmake)

# ---------------------------------------------------------------------------
# Configuration summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "useTracker ${PROJECT_VERSION} — ${CMAKE_BUILD_TYPE} — ${CMAKE_SYSTEM_NAME}")
message(STATUS "  OpenCV        ${OpenCV_VERSION}")
message(STATUS "  FFmpeg        libavutil ${FFmpeg_VERSION}")
message(STATUS "  ArucoColor    ${USE_ARUCO_COLOR}")
message(STATUS "  Bluetooth     ${USE_BLUETOOTH}")
message(STATUS "  Bundle deps   ${USETRACKER_BUNDLE_DEPS}")
message(STATUS "  Install to    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
