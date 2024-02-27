#!/bin/sh

# Format and normalize data for SMLP
src=mushroom_data_orig.csv
dest=mushroom_data.csv
formater=mushroom_formater.awk

echo "Formatting $src to $dest, please wait..."

# Print the file without its header to the formater and write the dest file
tail -n +2 $src | awk -f $formater > $dest

# Shuffle the lines of the dest file for better training
shuf $dest -o $dest

echo "done."