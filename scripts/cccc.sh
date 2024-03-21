#!/bin/sh


echo "warning: this script use cccc (apt-get install cccc)"
echo "see https://sarnold.github.io/cccc/CCCC_User_Guide.html"
echo "check the HTML report then at .cccc/cccc.html"
echo "there should be some false warnings with C++20 syntaxe\n"


cd ..
cccc libs/libsmlp/include/* libs/libsmlp/src/* src/*.cpp src/include/*.h
