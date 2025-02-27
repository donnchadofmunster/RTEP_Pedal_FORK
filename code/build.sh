#!/bin/bash

original_directory=$(pwd)
script_directory=$(dirname "$0")

cd "$script_directory"

mkdir -p build
cd build

# Run CMake and log errors
cmake -G "Unix Makefiles" .. 2>&1 | tee cmake_errors.log
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Check cmake_errors.log"
    exit 1
fi

make 2>&1 | tee make_errors.log
if [ $? -ne 0 ]; then
    echo "Build failed. Check make_errors.log"
    exit 1
fi

cd "$original_directory"
echo "Application built successfully"
