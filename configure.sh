#!/bin/bash

# directory where current shell script resides
PROJECTDIR=$(dirname "$BASH_SOURCE")

cd "$PROJECTDIR"

# If setup.sh was called before
if [ -d "libs/benchmark/googletest" ]; then
    rm -rf libs/benchmark
fi

# Initialize submodules if needed
git submodule init
# Update submodule if needed
git submodule update --remote

# Setup googletest
git clone https://github.com/google/googletest.git libs/benchmark/googletest
# Set google test to specific release tag
cd libs/benchmark/googletest
git fetch --all --tags --prune
git checkout tags/release-1.10.0 -b release-1.10.0
cd -

# Setup googlebenchmark
cd libs/benchmark
git fetch --all --tags --prune
git checkout tags/v1.5.0 -b v1.5.0
cd -

# Build google benchmark
cd libs/benchmark
mkdir -p build && cd build

if [ $(command -v ninja) != "" ]; then
    cmake ../ -GNinja
else
    cmake ../
fi
cmake --build . -- -j12
