#!/bin/bash
if ! command -v conan >> /dev/null
then
    echo "First Build ? Installing conan using pip"
    pip install conan==1.59.0
fi

mkdir -p bin
mkdir -p build && cd build
conan profile update settings.compiler.libcxx=libstdc++11 default
conan install .. --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --profile ../conanprofile_linux.txt
cmake .. -G "Ninja"
cmake --build .
