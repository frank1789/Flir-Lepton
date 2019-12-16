#!/usr/bin/env sh

rm -rf build
mkdir build && cd build
lcov --directory . --zerocounters
cmake -D CMAKE_BUILD_TYPE=Debug ..
make
EXECUTABLE=$(find $PWD -name "FlirLepton")
${EXECUTABLE}
