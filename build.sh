#!/usr/bin/env sh


rm -rf build
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Debug ..
make -j4
./FlirLepton
