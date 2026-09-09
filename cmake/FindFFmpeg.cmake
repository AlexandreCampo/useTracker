# FindFFmpeg.cmake
# Locate the FFmpeg libraries useTracker needs: libavformat, libavcodec,
# libswscale, libavutil.
#
# pkg-config is asked first — Linux distributions, Homebrew and vcpkg (with
# pkgconf) all ship .pc files, and they carry the right include/library dirs for
# non-standard prefixes such as /opt/homebrew. A plain header/library search is
# used as a fallback so the module still works without pkg-config (e.g. MSVC).
#
# Defines:
#   FFmpeg_FOUND
#   FFmpeg_INCLUDE_DIRS
#   FFmpeg_LIBRARIES
#   FFmpeg_LIBRARY_DIRS          (useful to seed macOS bundle fixup)
#   FFmpeg::avformat, FFmpeg::avcodec, FFmpeg::swscale, FFmpeg::avutil

include(FindPackageHandleStandardArgs)

find_package(PkgConfig QUIET)

set(_ffmpeg_components avformat avcodec swscale avutil)

foreach(_comp IN LISTS _ffmpeg_components)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(PC_${_comp} QUIET lib${_comp})
    endif()

    # libavformat/avformat.h, libavcodec/avcodec.h, libswscale/swscale.h, ...
    find_path(${_comp}_INCLUDE_DIR
        NAMES lib${_comp}/${_comp}.h
        HINTS ${PC_${_comp}_INCLUDE_DIRS}
        PATH_SUFFIXES ffmpeg
    )
    find_library(${_comp}_LIBRARY
        NAMES ${_comp}
        HINTS ${PC_${_comp}_LIBRARY_DIRS}
    )

    if(${_comp}_INCLUDE_DIR AND ${_comp}_LIBRARY)
        set(${_comp}_FOUND TRUE)
    endif()
endforeach()

# avutil carries the version we report; it is bumped in lockstep with the rest.
if(PC_avutil_VERSION)
    set(FFmpeg_VERSION "${PC_avutil_VERSION}")
endif()

find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS
        avformat_LIBRARY avformat_INCLUDE_DIR
        avcodec_LIBRARY  avcodec_INCLUDE_DIR
        swscale_LIBRARY  swscale_INCLUDE_DIR
        avutil_LIBRARY   avutil_INCLUDE_DIR
    VERSION_VAR FFmpeg_VERSION
)

if(FFmpeg_FOUND)
    set(FFmpeg_INCLUDE_DIRS "")
    set(FFmpeg_LIBRARIES    "")
    set(FFmpeg_LIBRARY_DIRS "")

    foreach(_comp IN LISTS _ffmpeg_components)
        list(APPEND FFmpeg_INCLUDE_DIRS "${${_comp}_INCLUDE_DIR}")
        list(APPEND FFmpeg_LIBRARIES    "${${_comp}_LIBRARY}")

        get_filename_component(_dir "${${_comp}_LIBRARY}" DIRECTORY)
        list(APPEND FFmpeg_LIBRARY_DIRS "${_dir}")

        if(NOT TARGET FFmpeg::${_comp})
            add_library(FFmpeg::${_comp} UNKNOWN IMPORTED)
            set_target_properties(FFmpeg::${_comp} PROPERTIES
                IMPORTED_LOCATION "${${_comp}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${${_comp}_INCLUDE_DIR}"
            )
        endif()
    endforeach()

    list(REMOVE_DUPLICATES FFmpeg_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES FFmpeg_LIBRARY_DIRS)
endif()

mark_as_advanced(
    avformat_INCLUDE_DIR avformat_LIBRARY
    avcodec_INCLUDE_DIR  avcodec_LIBRARY
    swscale_INCLUDE_DIR  swscale_LIBRARY
    avutil_INCLUDE_DIR   avutil_LIBRARY
)
