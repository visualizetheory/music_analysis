cmake_minimum_required(VERSION 3.10)
project(py_essentia_util)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# === Python executable from venv (must come before find_package) ===
set(Python_EXECUTABLE "${CMAKE_SOURCE_DIR}/../venv/bin/python")
set(PYBIND11_FINDPYTHON ON)

# === Now find Python and pybind11 ===
find_package(Python REQUIRED COMPONENTS Interpreter Development)

find_package(pybind11 REQUIRED
  PATHS "${CMAKE_SOURCE_DIR}/../venv/lib/python3.10/site-packages/pybind11/share/cmake/pybind11"
)

# === Paths ===
set(ESSENTIA_UTIL_ROOT "${CMAKE_SOURCE_DIR}/../essentia_util")
set(ESSENTIA_UTIL_LIB "${ESSENTIA_UTIL_ROOT}/build/libessentia_util.a")
set(ESSENTIA_UTIL_INCLUDE "${ESSENTIA_UTIL_ROOT}/include")

# === pybind11 module ===
add_library(py_essentia_util MODULE
    bindings/py_essentia_util.cpp
)

target_include_directories(py_essentia_util PRIVATE
    ${ESSENTIA_UTIL_INCLUDE}
    ${ESSENTIA_UTIL_INCLUDE}/eigen3
)

target_link_libraries(py_essentia_util
    PRIVATE
    pybind11::module
    ${ESSENTIA_UTIL_LIB}
)

set_target_properties(py_essentia_util PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)



