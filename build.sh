#!/usr/bin/env sh

# This script provides two functions:
#   - "compile_debug": check if there are folders containing previous builds.
#                    Compile the project in debug mode, then once completed,
#                    start the unit test.
#                    Collects information for code coverage.
#
#   - "compile_release": check if there are folders containing previous builds.
#                       Fill in the project and start the executable.
#
# Args:
#    - $1 the first argument is optional and is used to activate the debug mode.
#      Is required to specify "debug" or "debug" does not accept other arguments.
#
# Usage:
#
# compile in debug mode:
# $./build.sh debug
#
# # compile in release mode:
# $./build.sh
#

# ////////////////////////////////////////////////////////////////////////////
# declare function
# ////////////////////////////////////////////////////////////////////////////

compile_debug() {
    DIRECTORY=build_debug
    clear_old_build
    echo "create new directory $DIRECTORY"
    mkdir $DIRECTORY && cd $DIRECTORY
    cmake -D CMAKE_BUILD_TYPE=Debug ..
    make
    EXECUTABLE=$(find $PWD -name "FlirLepton")
    ${EXECUTABLE}
}

compile_release() {
    DIRECTORY=build_release
    clear_old_build
    echo "create new directory $DIRECTORY"
    mkdir $DIRECTORY && cd $DIRECTORY
    cmake -D CMAKE_BUILD_TYPE=Release ..
    make
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
    echo "**** build release mode ****"
    compile_release
else
    if [ "$1" = "Debug" ] || [ "$1" = "debug" ]; then
        echo "**** build debug mode ****"
        compile_debug
    fi
fi
