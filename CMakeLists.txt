cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(paradice-9)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_CXX_STANDARD 14)

include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate headers suitable for declaring which symbols
# should be included.
include(GenerateExportHeader)

add_subdirectory(odin)
add_subdirectory(munin)
add_subdirectory(paradice)
add_subdirectory(hugin)
add_subdirectory(paradice9)
# add_subdirectory(test)
