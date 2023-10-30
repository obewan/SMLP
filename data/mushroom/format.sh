#!/bin/sh

# Format for SMLP
src=mushroom_data_orig.csv
dest=mushroom_data.csv
formater=mushroom_formater.awk

echo "Formatting $src to $dest, please wait..."
tail -n +2 $src | awk -f $formater > $dest
shuf $dest -o $dest
echo "done."