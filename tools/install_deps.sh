#!/usr/bin/env bash

sudo apt-get update
sudo apt-get install -y clang++-15
sudo ln -sf /usr/bin/clang++-15 /usr/bin/c++
sudo ln -sf /usr/bin/clang++-15 /usr/bin/clang++
/usr/bin/c++ --version
/usr/bin/clang++ --version
/usr/bin/clang++-15 --version
pip install conan
#conan install . -of build -b="*"
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.compiler=clang default
conan profile update settings.compiler.version=15 default
conan profile update settings.build_type=$BUILD_TYPE default
##conan install --build missing -if ./build . -o gtest:asan=True
conan profile show default
conan install . -of build -b="*"