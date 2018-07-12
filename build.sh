#!/bin/bash
# Ubuntu supports multiple versions of clang to be installed at the same time.
# The tests need to determin the clang binary before calling cmake

CMAKE_PRIVATE_FLAGS=

TOOLCHAIN_FILE="cmake/arm-linux-toolchain.cmake"

function abort {
    echo "usage:"
    echo "       build.sh { clang | gcc } {Release | Debug}"
    exit
}

if [ -z "$1" ] || [ -z "$2" ] ; then
    abort
fi

if [ "$1" = "clang" ]; then
    CC_BIN=`which clang`
    CXX_BIN=`which clang++`
    if [ -z $CC_BIN ]; then
        clang_ver=`dpkg --get-selections | grep clang | grep -v -m1 libclang | cut -f1 | cut -d '-' -f2`
        CC_BIN="clang-$clang_ver"
        CXX_BIN="clang++-$clang_ver"
    fi
    echo "Will use clang [$CC_BIN] and clang++ [$CXX_BIN]"
    CMAKE_PRIVATE_FLAGS="$CMAKE_PRIVATE_FLAGS -DCMAKE_CXX_COMPILER=$CXX_BIN"
    echo $CMAKE_PRIVATE_FLAGS
elif [  "$1" = "gcc" ] ; then
    CC_BIN=`which gcc`
    CXX_BIN=`which g++`
    if [ -z $CC_BIN ]; then
        clang_ver=`dpkg --get-selections | grep gcc | grep -v -m1 libclang | cut -f1 | cut -d '-' -f2`
        CC_BIN="clang-$clang_ver"
        CXX_BIN="clang++-$clang_ver"
    fi
    echo "Will use clang [$CC_BIN] and clang++ [$CXX_BIN]"
    CMAKE_PRIVATE_FLAGS=$CMAKE_PRIVATE_FLAGS -DCMAKE_CXX_COMPILER=$CXX_BIN
    echo $CMAKE_PRIVATE_FLAGS
    echo "Will use gcc [$CC_BIN] and g++ [$CXX_BIN]"
elif [  "$1" = "arm" ] ; then
    echo "Building for arm with buildroot toolchain"
    CMAKE_PRIVATE_FLAGS="-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE"
else
    echo "Wrong compiler selected"
    abort
fi

mkdir -p build && cd build


if [ "$2" = "Release" ]; then
    cmake $CMAKE_PRIVATE_FLAGS -DCMAKE_BUILD_TYPE=Release ../
elif [  "$2" = "Debug" ] ; then
    cmake $CMAKE_PRIVATE_FLAGS -DCMAKE_BUILD_TYPE=Debug ../
else
    echo "Wrong build configuration selected"
    abort
fi



if [ "$2" = "Release" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release $CMAKE_PRIVATE_FLAGS ../
elif [  "$2" = "Debug" ] ; then
    cmake -DCMAKE_BUILD_TYPE=Debug $CMAKE_PRIVATE_FLAGS ../
else
    echo "Wrong build configuration selected"
    abort
fi


make -j 8


