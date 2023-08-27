#!/bin/sh
# require cmake

# configure build files
cmake -B ./build

# generate build files
cmake --build ./build

# compile only
#cd ./build; make; cd ..;

# run
#./build/smlp
