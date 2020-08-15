#!/bin/bash

# relative directory where current shell script resides from where shell script was called
PROJECTDIR="$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
echo "Project directory: $PROJECTDIR"

cd "$PROJECTDIR"

# Install nlohmann/json
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    if [ $(command -v conan) == "" ]; then
        echo "config fail: conan not installed"
        exit 1
    fi
    cd libs
    conan install .
    cd -
elif [[ "$OSTYPE" == "darwin"* ]]; then
    if [ $(command -v brew) == "" ]; then
        echo "config fail: brew not installed"
        exit 1
    fi
    brew install nlohmann-json
else
    echo "config fail: unrecognizable OS"
fi

# If setup.sh was called before
if [ -d "libs/benchmark/googletest" ]; then
    rm -rf libs/benchmark
fi

# Initialize submodules if needed
git submodule update --init
# Update submodule if needed
git submodule update --recursive --remote

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

# Build and install google benchmark locally
cd libs/benchmark
mkdir -p build && cd build
cmake_flags="-DCMAKE_INSTALL_PREFIX=$PROJECTDIR/libs/benchmark/build"
if [ $(command -v ninja) != "" ]; then
    cmake ../ -GNinja $cmake_flags "$@"
else
    cmake ../ $cmake_flags "$@"
fi
cmake --build . --target install -- -j12
