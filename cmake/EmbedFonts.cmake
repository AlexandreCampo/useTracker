# Keep typography available in standalone executables and every packaged build.
# Generate C++ arrays at configure time; never require a font on the host system.
set(_font_source "// Generated from ui/fonts. See ui/fonts/OFL.txt.\n")
foreach(_family Sans Mono)
    set(_font "${CMAKE_CURRENT_SOURCE_DIR}/ui/fonts/Liberation${_family}-Regular.ttf")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_font}")
    file(READ "${_font}" _bytes HEX)
    string(REGEX REPLACE "(..)" "0x\\1," _bytes "${_bytes}")
    string(APPEND _font_source "extern const unsigned char Darkroom${_family}Data[] = {${_bytes}};\nextern const int Darkroom${_family}Size = sizeof(Darkroom${_family}Data);\n")
endforeach()
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/DarkroomFonts.cpp" CONTENT "${_font_source}")
unset(_font_source)
unset(_bytes)
