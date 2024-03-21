#!/bin/sh


echo "warning: this script require Lizard (Python: pip install lizard (or use pipx))"
echo "see https://github.com/terryyin/lizard \n"

cd ..
lizard ./src/*.cpp ./src/include/SimpleMLP.h ./libs/libsmlp/