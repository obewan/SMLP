#!/bin/sh

# valgrind is required (apt-get install valgrind)
# this test will take around 10 minutes (as it is slower under valdrind)
# check the memory report then at .valgrind/valgrind-out.txt
cd build
valgrind --leak-check=full \
  --show-leak-kinds=all \
  --track-origins=yes \
  --verbose \
  --log-file=../.valgrind/valgrind-out.txt \
  ./smlp  -e "myMushroomMLP.json" -f "../data/mushroom/mushroom_data.csv" -s 20 -o 1 -c 1 -d 12 -p 2 -a "ReLU" -m "TrainTestMonitored"
