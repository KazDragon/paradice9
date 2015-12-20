cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(paradice-9)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(FindBoost)
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

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate headers suitable for declaring which symbols
# should be included.
include(GenerateExportHeader)

# Due to the strange requirements for Google Test, it is assumed to have been
# built in the "gtest" directory.  For this, it is required to run the moral
# equivalent of the following script before building Telnet++:
#
#  mkdir gtest
#  cd gtest
#  cmake /usr/src/gtest && make
#  export GTEST_ROOT=$PWD
#  cd ..
find_package(GTest)

# For GTest, it is necessary to link with the appropriate local threading 
# library.
find_package(Threads)

if (GTEST_FOUND)
    enable_testing()
endif()

add_subdirectory(telnetpp)
add_subdirectory(terminalpp)
add_subdirectory(odin)
add_subdirectory(munin)
add_subdirectory(paradice)
add_subdirectory(hugin)
add_subdirectory(paradice9)
add_subdirectory(test)
