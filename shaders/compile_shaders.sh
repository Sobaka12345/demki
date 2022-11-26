#!/bin/bash

input_dir=$1
output_dir=$2

mkdir $output_dir &> /dev/null

for file in $input_dir/*{.vert,.frag};
do
    fileName="${file##*/}";
    glslc $input_dir/$fileName -o "$output_dir/$fileName"".spv"
done;
