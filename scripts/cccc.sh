#!/bin/sh

# cccc is required (apt-get install cccc)
# check the HTML report then at .cccc/cccc.html

cd ..
cccc libs/libsmlp/include/* libs/libsmlp/src/* src/*.cpp src/include/*.h
