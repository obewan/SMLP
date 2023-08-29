#!/bin/sh

# Format for SMLP
tail -n +2 mushroom_data_orig.csv | awk -f formater.awk > mushroom_data.csv
