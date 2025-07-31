cmake_minimum_required(VERSION 3.10)
project(essentia_util)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # Required for pybind11 bindings

# === Paths ===
set(ESSENTIA_PREFIX "$ENV{HOME}/essentia_deps_static")

# === Includes ===
include_directories(
    include
    ${ESSENTIA_PREFIX}/include
    ${ESSENTIA_PREFIX}/include/eigen3
)

# === Object library ===
add_library(essentia_util_obj OBJECT
    src/essentia_util.cpp
)

# === Manually construct the final static library with all dependencies ===
set(ESSENTIA_UTIL_STATIC_PATH "${CMAKE_BINARY_DIR}/libessentia_util.a")

add_custom_command(
  OUTPUT ${ESSENTIA_UTIL_STATIC_PATH}
  COMMAND libtool -static -o ${ESSENTIA_UTIL_STATIC_PATH}
    $<TARGET_OBJECTS:essentia_util_obj>
    ${ESSENTIA_PREFIX}/lib/libessentia.a
    ${ESSENTIA_PREFIX}/lib/libavresample.a
    ${ESSENTIA_PREFIX}/lib/libavcodec.a
    ${ESSENTIA_PREFIX}/lib/libavformat.a
    ${ESSENTIA_PREFIX}/lib/libavutil.a
    ${ESSENTIA_PREFIX}/lib/libavfilter.a
    ${ESSENTIA_PREFIX}/lib/libavdevice.a
    ${ESSENTIA_PREFIX}/lib/libswresample.a
    ${ESSENTIA_PREFIX}/lib/libswscale.a
    ${ESSENTIA_PREFIX}/lib/libpostproc.a
    ${ESSENTIA_PREFIX}/lib/libsamplerate.a
    ${ESSENTIA_PREFIX}/lib/libfftw3f.a
    ${ESSENTIA_PREFIX}/lib/libfftw3.a
    ${ESSENTIA_PREFIX}/lib/libtag.a
    ${ESSENTIA_PREFIX}/lib/libtag_c.a
    ${ESSENTIA_PREFIX}/lib/libyaml.a
    ${ESSENTIA_PREFIX}/lib/libz.a
    ${ESSENTIA_PREFIX}/lib/libiconv.a
    ${ESSENTIA_PREFIX}/lib/libcharset.a
  DEPENDS essentia_util_obj
  VERBATIM
)

add_custom_target(essentia_util ALL DEPENDS ${ESSENTIA_UTIL_STATIC_PATH})

# === Declare the .a file as an IMPORTED library ===
add_library(libessentia_util STATIC IMPORTED GLOBAL)
set_target_properties(libessentia_util PROPERTIES
    IMPORTED_LOCATION ${ESSENTIA_UTIL_STATIC_PATH}
)






