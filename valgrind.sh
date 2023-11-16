#!/bin/sh

cd build
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --verbose \
  --log-file=../.valgrind/valgrind-out.txt \
  ./smlp  -e "myMushroomMLP.json" -f "../data/mushroom/mushroom_data.csv" -s 20 -o 1 -c 1 -d 12 -p 2 -a "ReLU" -m "TrainTestMonitored"
