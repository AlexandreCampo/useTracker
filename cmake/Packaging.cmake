# Packaging.cmake — CPack configuration for every platform useTracker targets.
#
#   Linux    TGZ, DEB, RPM (RPM only when rpmbuild is available)
#   macOS    DragNDrop (.dmg), TGZ
#   Windows  ZIP, NSIS installer (NSIS only when makensis is available)
#
# Run after building:   cpack --config <build-dir>/CPackConfig.cmake
# or simply:            cmake --build <build-dir> --target package
#
# The AppImage — the artifact that runs on any reasonably recent distribution —
# is produced by packaging/linux/build-appimage.sh, not by CPack.

set(CPACK_PACKAGE_NAME                "useTracker")
set(CPACK_PACKAGE_VENDOR              "Alexandre Campo")
set(CPACK_PACKAGE_CONTACT             "Alexandre Campo <alexandre.campo@gmail.com>")
set(CPACK_PACKAGE_VERSION             "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_HOMEPAGE_URL        "${PROJECT_HOMEPAGE_URL}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "useTracker")
set(CPACK_RESOURCE_FILE_LICENSE       "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README        "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_EXECUTABLES         "useTracker;useTracker")
set(CPACK_STRIP_FILES                 TRUE)
set(CPACK_VERBATIM_VARIABLES          TRUE)

# --- artifact name: useTracker-2.0.0-Linux-x86_64 --------------------------
if(APPLE AND CMAKE_OSX_ARCHITECTURES)
    string(REPLACE ";" "-" _pkg_arch "${CMAKE_OSX_ARCHITECTURES}")
elseif(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_pkg_arch "x64")
else()
    set(_pkg_arch "${CMAKE_SYSTEM_PROCESSOR}")
endif()
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${_pkg_arch}")

# ---------------------------------------------------------------------------
# Linux
# ---------------------------------------------------------------------------
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ")

    # A .deb built on Ubuntu 22.04 and one built on 24.04 link against different
    # OpenCV sonames, so the distribution has to be part of the file name —
    # otherwise the two overwrite each other on the release page.
    if(EXISTS /etc/os-release)
        file(READ /etc/os-release _os_release)
        string(REGEX MATCH "\nID=\"?([a-z]+)\"?" _ "${_os_release}")
        set(_distro_id "${CMAKE_MATCH_1}")
        string(REGEX MATCH "\nVERSION_ID=\"?([0-9.]+)\"?" _ "${_os_release}")
        set(_distro_version "${CMAKE_MATCH_1}")
        if(_distro_id AND _distro_version)
            set(_distro "${_distro_id}${_distro_version}")
            set(CPACK_PACKAGE_FILE_NAME
                "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${_distro}-${_pkg_arch}")
            set(CPACK_DEBIAN_PACKAGE_RELEASE "1~${_distro}")
            set(CPACK_RPM_PACKAGE_RELEASE    "1.${_distro_id}${_distro_version}")
        endif()
    endif()

    find_program(DPKG_EXECUTABLE dpkg)
    if(DPKG_EXECUTABLE)
        list(APPEND CPACK_GENERATOR "DEB")
    endif()

    find_program(RPMBUILD_EXECUTABLE rpmbuild)
    if(RPMBUILD_EXECUTABLE)
        list(APPEND CPACK_GENERATOR "RPM")
    endif()

    # --- Debian / Ubuntu ---------------------------------------------------
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION    "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY   "optional")
    set(CPACK_DEBIAN_FILE_NAME          "DEB-DEFAULT")
    # Let dpkg-shlibdeps derive the exact OpenCV/FFmpeg/SDL2 versions from the
    # binary, so the .deb declares what it was actually built against.
    find_program(DPKG_SHLIBDEPS_EXECUTABLE dpkg-shlibdeps)
    if(DPKG_SHLIBDEPS_EXECUTABLE)
        set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    else()
        set(CPACK_DEBIAN_PACKAGE_DEPENDS
            "libopencv-dev, libsdl2-2.0-0, libavformat-dev, libavcodec-dev, libswscale-dev, libavutil-dev")
    endif()

    # --- Fedora / openSUSE -------------------------------------------------
    set(CPACK_RPM_PACKAGE_LICENSE  "GPLv3+")
    set(CPACK_RPM_PACKAGE_GROUP    "Applications/Engineering")
    set(CPACK_RPM_PACKAGE_URL      "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_RPM_FILE_NAME        "RPM-DEFAULT")
    set(CPACK_RPM_PACKAGE_AUTOREQ  ON)
    # Directories owned by the filesystem/hicolor-icon-theme packages
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
        /usr/share/applications
        /usr/share/metainfo
        /usr/share/icons
        /usr/share/icons/hicolor
        /usr/share/icons/hicolor/scalable
        /usr/share/icons/hicolor/scalable/apps
    )

# ---------------------------------------------------------------------------
# macOS
# ---------------------------------------------------------------------------
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
    set(CPACK_DMG_VOLUME_NAME "useTracker ${PROJECT_VERSION}")
    set(CPACK_DMG_FORMAT      "UDZO")

# ---------------------------------------------------------------------------
# Windows
# ---------------------------------------------------------------------------
elseif(WIN32)
    set(CPACK_GENERATOR "ZIP")

    find_program(MAKENSIS_EXECUTABLE makensis)
    if(MAKENSIS_EXECUTABLE)
        list(APPEND CPACK_GENERATOR "NSIS")
    endif()

    set(CPACK_NSIS_PACKAGE_NAME       "useTracker")
    set(CPACK_NSIS_DISPLAY_NAME       "useTracker ${PROJECT_VERSION}")
    set(CPACK_NSIS_URL_INFO_ABOUT     "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_NSIS_CONTACT            "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "useTracker.exe")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH        OFF)
    if(EXISTS "${CMAKE_SOURCE_DIR}/packaging/icons/useTracker.ico")
        set(CPACK_NSIS_MUI_ICON   "${CMAKE_SOURCE_DIR}/packaging/icons/useTracker.ico")
        set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/packaging/icons/useTracker.ico")
    endif()
endif()

# ---------------------------------------------------------------------------
# Source tarball (`cpack --config CPackSourceConfig.cmake`), used by the
# Arch PKGBUILD and by anyone packaging a release from source.
# ---------------------------------------------------------------------------
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "useTracker-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\\\.git/"
    "/\\\\.github/"
    "/build.*/"
    "/dist/"
    "/\\\\.cache/"
    "\\\\.MP4$"
    "\\\\.mp4$"
    "\\\\.o$"
    "\\\\.a$"
    "compile_commands\\\\.json$"
    "imgui\\\\.ini$"
)

include(CPack)
