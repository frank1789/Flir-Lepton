#!/usr/bin/env sh



# ////////////////////////////////////////////////////////////////////////////
# declare function
# ////////////////////////////////////////////////////////////////////////////

compile_debug() {
    DIRECTORY=build_debug
    clear_old_build
    echo "create new directory $DIRECTORY"
    mkdir $DIRECTORY && cd $DIRECTORY
    cmake -D CMAKE_BUILD_TYPE=Debug ..
    make -j$(nproc)
    EXECUTABLE=$(find $PWD -name "FlirLepton")
    ${EXECUTABLE}
}

compile_release() {
    DIRECTORY=build_release
    clear_old_build
    echo "create new directory $DIRECTORY"
    mkdir $DIRECTORY && cd $DIRECTORY
    cmake -D CMAKE_BUILD_TYPE=Release ..
    make -j$(nproc)
    EXECUTABLE=$(find $PWD -name "FlirLepton")
    ${EXECUTABLE}
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

if [ "$#" -eq "0" ]; then
    echo "build release version"
    compile_release
else
    if [ "$1" = "Debug" ] || [ "$1" = "debug" ]; then
        echo "build release version"
        compile_debug
    fi
fi
