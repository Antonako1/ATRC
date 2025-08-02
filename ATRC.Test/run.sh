#!/bin/bash
cd "$(dirname "$0")"
set -e

rm -rf build
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
cmake --build .
