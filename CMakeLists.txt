cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)
project(paradice-9)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(cotire OPTIONAL)

find_package(Boost
    REQUIRED
    COMPONENTS
        filesystem
        program_options
        random
        serialization
        signals
        system)

# Crypto++ gives us the ability to hash passwords so that they can't be easily
# discovered by hacking at character files.  However, it's not strictly
# required.
find_package(Crypto++)

# CppUnit is required for the unit testing.
find_package(CppUnit)

if (CPPUNIT_FOUND)
    enable_testing()
endif()

add_subdirectory(telnetpp)
add_subdirectory(odin)
add_subdirectory(munin)
add_subdirectory(paradice)
add_subdirectory(hugin)
add_subdirectory(paradice9)
add_subdirectory(test)
