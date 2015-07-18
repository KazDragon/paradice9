cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)
project(paradice-9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost 
    REQUIRED
    COMPONENTS
        filesystem
        program_options
        random
        serialization
        signals
        system)

find_package(Crypto++ REQUIRED)

enable_testing()

add_subdirectory(odin)
add_subdirectory(munin)
add_subdirectory(paradice)
add_subdirectory(hugin)
add_subdirectory(paradice9)
add_subdirectory(test)
