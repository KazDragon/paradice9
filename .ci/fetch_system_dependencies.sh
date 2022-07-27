#!/bin/bash
if [ -z ${EXTERNAL_ROOT+x} ]; then echo EXTERNAL_ROOT not set; exit 1; fi

export EXTERNAL_BUILD_ROOT=$HOME/external_build

mkdir "$EXTERNAL_BUILD_ROOT" || true

# Install nlohmann_json dependency
if [ ! -f "$EXTERNAL_ROOT/include/nlohmann/json.hpp" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/nlohmann/json/archive/v3.3.0.tar.gz;
    tar -xzf v3.3.0.tar.gz;
    cd json-3.3.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DJSON_BuildTests=Off .;
    make -j2 && make install;
fi

# Install fmt dependency
if [ ! -f "$EXTERNAL_ROOT/include/fmt/format.hpp" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/fmtlib/fmt/archive/7.1.2.tar.gz;
    tar -xzf 7.1.2.tar.gz;
    cd fmt-7.1.2;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DFMT_TEST=Off .;
    make -j2 && make install;
fi

# Install gsl-lite dependency
if [ ! -f "$EXTERNAL_ROOT/include/gsl/gsl-lite.hpp" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/gsl-lite/gsl-lite/archive/v0.38.0.tar.gz;
    tar -xzf v0.38.0.tar.gz;
    cd gsl-lite-0.38.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DGSL_LITE_OPT_BUILD_TESTS=Off .;
    make -j2 && make install;
fi

# Install SQLiteCpp dependency
if [ ! -f "$EXTERNAL_ROOT/include/SQLiteCpp/SQLiteCpp.h" ]; then
    wget https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.1.1.tar.gz;
    tar -xzf 3.1.1.tar.gz;
    cd SQLiteCpp-3.1.1
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" .;
    make -j2 && make install;
    cd ..;
fi

# Install cryptopp dependency
if [ ! -f "$EXTERNAL_ROOT/include/cryptopp/sha.h" ]; then
    wget https://github.com/weidai11/cryptopp/archive/refs/tags/CRYPTOPP_8_6_0.tar.gz;
    tar -xzf CRYPTOPP_8_6_0.tar.gz
    cd cryptopp-CRYPTOPP_8_6_0;
    make install PREFIX="$EXTERNAL_ROOT"
    cd ..;
fi

# Install gtest dependency
if [ ! -f "$EXTERNAL_ROOT/include/gtest/gtest.h" ]; then
    cd "$EXTERNAL_BUILD_ROOT";
    wget https://github.com/google/googletest/archive/release-1.10.0.tar.gz;
    tar -xzf release-1.10.0.tar.gz;
    cd googletest-release-1.10.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" .;
    make -j2 && make install;
fi

# Install Server++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/serverpp/version.hpp" ]; then
    wget https://github.com/KazDragon/serverpp/archive/refs/tags/v0.2.0.tar.gz;
    tar -xzf v0.2.0.tar.gz;
    cd serverpp-0.2.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DSERVERPP_WITH_TESTS=False -DSERVERPP_VERSION="0.2.0" .;
    make -j2 && make install;
    cd ..;
fi

# Install Telnet++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/telnetpp/version.hpp" ]; then
    wget https://github.com/KazDragon/telnetpp/archive/v3.0.0.tar.gz -O - | tar xz;
    cd telnetpp-3.0.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DTELNETPP_WITH_TESTS=False -DTELNETPP_WITH_ZLIB=ON -DTELNETPP_VERSION="3.0.0" .;
    make -j2 && make install;
    cd ..;
fi

# Install Terminal++ dependency
if [ ! -f "$EXTERNAL_ROOT/include/terminalpp/version.hpp" ]; then
    wget https://github.com/KazDragon/terminalpp/archive/v3.0.0.tar.gz -O - | tar xz;
    cd terminalpp-3.0.0;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DTERMINALPP_WITH_TESTS=False -DTERMINALPP_VERSION="3.0.0" .;
    make -j2 && make install;
    cd ..;
fi

# Install Munin dependency
if [ ! -f "$EXTERNAL_ROOT/include/munin/version.hpp" ]; then
    wget https://github.com/KazDragon/munin/archive/refs/tags/v0.7.1.tar.gz;
    tar -xzf v0.7.1.tar.gz;
    cd munin-0.7.1;
    cmake -DCMAKE_INSTALL_PREFIX="$EXTERNAL_ROOT" -DCMAKE_PREFIX_PATH="$EXTERNAL_ROOT" -DMUNIN_WITH_TESTS=False -DMUNIN_VERSION="0.7.1" .;
    make -j2 && make install;
    cd ..;
fi
