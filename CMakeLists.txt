cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(paradice-9)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate headers suitable for declaring which symbols
# should be included.
include(GenerateExportHeader)

add_subdirectory(paradice)
# add_subdirectory(hugin)
add_subdirectory(paradice9)
# add_subdirectory(test)
