#!/bin/bash
if [ -z ${EXTERNAL_ROOT+x} ]; then echo EXTERNAL_ROOT not set; exit 1; fi

export EXTERNAL_BUILD_ROOT=$HOME/external_build

mkdir "$EXTERNAL_BUILD_ROOT" || true

# Note: libfmt is installed from apt in build.yml.

# Install nlohmann_json dependency
if [ ! -f "$EXTERNAL_ROOT/include/nlohmann/json.hpp" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/nlohmann/json/archive/v3.3.0.tar.gz -O - | tar xz;
    cd json-3.3.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DJSON_BuildTests=Off .;
    cmake --build . -j2 --target=install;
    cd ..
fi

# Install gsl-lite dependency
if [ ! -f "$EXTERNAL_ROOT/include/gsl/gsl-lite.hpp" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/gsl-lite/gsl-lite/archive/v0.38.0.tar.gz -O - | tar xz;
    cd gsl-lite-0.38.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DGSL_LITE_OPT_BUILD_TESTS=Off .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

# Install SQLiteCpp dependency
if [ ! -f "$EXTERNAL_ROOT/include/SQLiteCpp/SQLiteCpp.h" ]; then
    wget https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.1.1.tar.gz -O - | tar xz;
    cd SQLiteCpp-3.1.1
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

# Install Crypto++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/cryptopp/cryptlib.h" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/weidai11/cryptopp/archive/refs/tags/CRYPTOPP_8_6_0.tar.gz -O - | tar xz;
    cd cryptopp-CRYPTOPP_8_6_0;
    wget -O CMakeLists.txt https://raw.githubusercontent.com/noloader/cryptopp-cmake/master/CMakeLists.txt;
    wget -O cryptopp-config.cmake https://raw.githubusercontent.com/noloader/cryptopp-cmake/master/cryptopp-config.cmake;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DBUILD_TESTING=OFF -DCRYPTOPP_DATA_DIR="" .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

# Install gtest dependency
if [ ! -f "$EXTERNAL_ROOT/include/gtest/gtest.h" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/google/googletest/archive/release-1.11.0.tar.gz -O - | tar xz;
    cd googletest-release-1.11.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" .;
    cmake --build . -j2 --target=install;
    cd ..
fi

# Install Server++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/serverpp-0.2.0/serverpp/version.hpp" ]; then
    wget https://github.com/KazDragon/serverpp/archive/refs/tags/v0.2.0.tar.gz -O - | tar xz;
    cd serverpp-0.2.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DSERVERPP_WITH_TESTS=False -DSERVERPP_VERSION="0.2.0" .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

# Install Telnet++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/telnetpp-3.1.0/telnetpp/version.hpp" ]; then
    wget https://github.com/KazDragon/telnetpp/archive/v3.1.0.tar.gz -O - | tar xz;
    cd telnetpp-3.1.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DTELNETPP_WITH_TESTS=False -DTELNETPP_WITH_ZLIB=ON -DTELNETPP_VERSION="3.0.0" .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

# Install Terminal++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/terminalpp-3.1.0/terminalpp/version.hpp" ]; then
    wget https://github.com/KazDragon/terminalpp/archive/v3.1.0.tar.gz -O - | tar xz;
    cd terminalpp-3.1.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DTERMINALPP_WITH_TESTS=False -DTERMINALPP_VERSION="3.0.2" .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

# Install Munin dependency
if [ ! -f "$EXTERNAL_ROOT/include/munin-0.7.2/munin/version.hpp" ]; then
    wget https://github.com/KazDragon/munin/archive/refs/tags/v0.7.2.tar.gz -O - | tar xz;
    cd munin-0.7.2;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DMUNIN_WITH_TESTS=False -DMUNIN_VERSION="0.7.1" .;
    cmake --build . -j2 --target=install;
    cd ..;
fi

echo Finished installing dependencies.
