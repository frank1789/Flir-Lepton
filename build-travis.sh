#!/usr/bin/env sh

compile_release() {
    DIRECTORY=build
    clear_old_build
    echo "create new directory $DIRECTORY"
    mkdir $DIRECTORY && cd $DIRECTORY
    cmake -D CMAKE_BUILD_TYPE=Release ..
    make
}

clear_old_build() {
    DIR_BUILD_DEBUG=build_debug
    DIR_BUILD_RELES=build_release
    # check for previuous debug build
    if [ -d "$DIR_BUILD_DEBUG" ]; then
        echo "remove previus build debug"
        rm -rf $DIR_BUILD_DEBUG

    # check for previus release build
    elif [ -d "$DIR_BUILD_RELES" ]; then
        echo "remove previus build release"
        rm -rf $DIR_BUILD_RELES
    fi

    if [ -d "build" ]; then
        echo "remove previus build"
        rm -rf build
    fi
}

# ////////////////////////////////////////////////////////////////////////////
# main script
# ////////////////////////////////////////////////////////////////////////////
compile_release

