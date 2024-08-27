#!/bin/bash

build_conf=$1

if ! command -v python3 &> /dev/null; then
    echo "python3 could not be found"
    exit
fi;

if [[ ${build_conf} != "Debug" ]] && [[ ${build_conf} != "Release" ]] && [[ ${build_conf} != "RelWithDebInfo" ]] && [[ ${build_conf} != "MinSizeRel" ]]; then
    build_conf="Debug"
    echo "Using default build type: $build_conf"
fi;

python3 -m venv build_env
source ./build_env/bin/activate

pip3 install --upgrade pip
pip3 install conan

conan install . -of build -pr:b default -pr:h wasm -s build_type=$build_conf --build=missing
