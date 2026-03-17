# FindFFmpeg.cmake
# Locate FFmpeg libraries: libavformat, libavcodec, libswscale, libavutil
#
# Defines:
#   FFmpeg_FOUND
#   FFmpeg_INCLUDE_DIRS
#   FFmpeg_LIBRARIES
#   FFmpeg::avformat, FFmpeg::avcodec, FFmpeg::swscale, FFmpeg::avutil (imported targets)

include(FindPackageHandleStandardArgs)

# Helper macro to find individual FFmpeg components
macro(_ffmpeg_find_component _component _header)
    find_path(${_component}_INCLUDE_DIR
        NAMES ${_header}
        PATH_SUFFIXES ffmpeg
    )
    find_library(${_component}_LIBRARY
        NAMES ${_component}
    )
    if(${_component}_INCLUDE_DIR AND ${_component}_LIBRARY)
        set(${_component}_FOUND TRUE)
    endif()
endmacro()

_ffmpeg_find_component(avformat libavformat/avformat.h)
_ffmpeg_find_component(avcodec  libavcodec/avcodec.h)
_ffmpeg_find_component(swscale  libswscale/swscale.h)
_ffmpeg_find_component(avutil   libavutil/avutil.h)

find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS
        avformat_LIBRARY avformat_INCLUDE_DIR
        avcodec_LIBRARY  avcodec_INCLUDE_DIR
        swscale_LIBRARY  swscale_INCLUDE_DIR
        avutil_LIBRARY   avutil_INCLUDE_DIR
)

if(FFmpeg_FOUND)
    set(FFmpeg_INCLUDE_DIRS
        ${avformat_INCLUDE_DIR}
        ${avcodec_INCLUDE_DIR}
        ${swscale_INCLUDE_DIR}
        ${avutil_INCLUDE_DIR}
    )
    list(REMOVE_DUPLICATES FFmpeg_INCLUDE_DIRS)

    set(FFmpeg_LIBRARIES
        ${avformat_LIBRARY}
        ${avcodec_LIBRARY}
        ${swscale_LIBRARY}
        ${avutil_LIBRARY}
    )

    # Create imported targets
    foreach(_comp avformat avcodec swscale avutil)
        if(NOT TARGET FFmpeg::${_comp})
            add_library(FFmpeg::${_comp} UNKNOWN IMPORTED)
            set_target_properties(FFmpeg::${_comp} PROPERTIES
                IMPORTED_LOCATION "${${_comp}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${${_comp}_INCLUDE_DIR}"
            )
        endif()
    endforeach()
endif()

mark_as_advanced(
    avformat_INCLUDE_DIR avformat_LIBRARY
    avcodec_INCLUDE_DIR  avcodec_LIBRARY
    swscale_INCLUDE_DIR  swscale_LIBRARY
    avutil_INCLUDE_DIR   avutil_LIBRARY
)
