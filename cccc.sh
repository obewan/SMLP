#!/bin/sh

# cccc is required (apt-get install cccc)
# check the report then at .cccc/cccc.html
cccc libs/libsmlp/include/* libs/libsmlp/src/* src/*.cpp src/include/*.h
