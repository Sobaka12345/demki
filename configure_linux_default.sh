#!/bin/bash

build_conf=$1

if ! command -v python3 &> /dev/null; then
    echo "python3 could not be found"
    exit
fi;

if [[ ${build_conf} != "Debug" ]] && [[ ${build_conf} != "Release" ]]; then
    build_conf="Debug"
    echo "Using default build type: $build_conf"
fi;

python3 -m venv build_env
source ./build_env/bin/activate

pip3 install conan

conan install . -if build/$build_conf/install -of build/$build_conf --build -s build_type=$build_conf
