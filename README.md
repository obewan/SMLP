# SMLP

A Simple Multi-Layer Perceptron

# Usage

- Prepare a dataset into a csv format with only float numbers from range 0 to 1 (example in test/mushroom we use a simple awk script to format mushroom_data_orig.csv into mushroom_data.csv)
- build and run smlp (should be in build directory then)
- to get command line help: `smlp -h`
- the `input_size` parameter (`-i` parameter) should be equal to the number of input data of your dataset, and the `output_size` (`-o` parameter) to the output data of your dataset. input_size + output_size should be equal to your data fields number. If your output is at the beginning of your data rows, you can indicate it with the `output_ends` parameter (`-z` parameter).
- `epochs` (`-e` paramater) will retrain the neural network with the dataset but after a backward propagation correction. It is better to set at least 100 epochs.
- the line_to number delimiter (`-l` parameter) is the line number from which smlp will use the remain dataset lines for test and not for train, so the first part is used for training and the second part of the file for testing. It should be a valid number, and be at around 70% of the datafile
- the `learning_rate` (`-r` parameter) should be small enought to have a fine training but big enought to be efficient, a value of 0.01 is recommanded with the mushroom example.

# Example

`smlp -f ../test/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100 -l 40000 -r 0.01 -v false`

## License

[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg

- _Attribution_ — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- _NonCommercial_ — You may not use the material for commercial purposes.
- _ShareAlike_ — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
- _No additional restrictions_ — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
