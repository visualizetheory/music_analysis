cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Paths ===
set(ESSENTIA_UTIL_ROOT "${CMAKE_SOURCE_DIR}/..")  # Points to essentia_util/
set(ESSENTIA_UTIL_LIB "${ESSENTIA_UTIL_ROOT}/build/libessentia_util.a")
set(ESSENTIA_PREFIX "$ENV{HOME}/essentia_deps_static")

# === Include directories ===
include_directories(
    ${ESSENTIA_UTIL_ROOT}/include
    ${ESSENTIA_PREFIX}/include/
    ${ESSENTIA_PREFIX}/include/eigen3    
)

# === Executable ===
add_executable(test main.cpp)

# === Link statically against libessentia_util.a ===
target_link_libraries(test
    ${ESSENTIA_UTIL_LIB}
    pthread
    bz2
)

# === macOS frameworks ===
if(APPLE)
    target_link_libraries(test
        "-framework CoreFoundation"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework Accelerate"
        "-framework CoreVideo"
    )
    target_link_options(test PRIVATE
        "-Wl,-alias,_libiconv,_iconv"
        "-Wl,-alias,_libiconv_open,_iconv_open"
        "-Wl,-alias,_libiconv_close,_iconv_close"
    )
endif()

