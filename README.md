# SMLP

A Simple Multi-Layer Perceptron, by [Dams](https://dams-labs.net/).  
Each neurons of a previous layer is connected with each neurons of its next layer.  
The neural network currently uses a sigmoid activation function.

# Features

- Simplicity (the "S" of SMLP) for better understanding and for studying purpose, for improved maintainance and performance, and for compliance with best software development practices (low complexity, KISS principle). It can also be used as base for more complex projects.
- Functionality: operational, including forward propagation, backward propagation and weight update.
- Performance: uses C++ for optimal performance, and less CPU and memory usage.
- CSV parsing using [csv-parser](https://github.com/ashaduri/csv-parser), dataset learning on the fly line by line (instead of loading the entire dataset into memory), resulting in low memory usage.
- Command line parsing using [CLI11](https://github.com/CLIUtils/CLI11).
- [Cmake](https://cmake.org) compilation, tested on Linux, but should be portable to other systems.
- [Doctest](https://github.com/doctest/doctest) unit tests.
- [Gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) and [Lcov](https://github.com/linux-test-project/lcov) code coverage.
- Github [Continuous Integration](https://docs.github.com/en/actions/automating-builds-and-tests/about-continuous-integration) with an unit test pass.
- Github [CodeQL](https://github.com/features/security/code) security analysis.
- [Sonarlint](https://www.sonarsource.com/products/sonarlint) code analysis.
- [Doxygen](https://www.doxygen.nl) documentation.
- Includes a mushroom edibility dataset example.

# Roadmap to first release

- Add import/export of the model.`[in progress]`
- Add more activation functions that can be selected.
- Improve code coverage to 80%
- Add interactive testing.
- Add an auto-training feature (searching for the best parameters).
- Add a GUI (but probably on a more advanced project).
- Add a CUDA support (but probably on a more advanced project).

# Usage

1. Prepare a dataset in CSV format with comma separation and with only float numbers ranging from 0 to 1 (example in test/mushroom we use a simple awk script to format mushroom_data_orig.csv into mushroom_data.csv).
2. Build and run smlp (should be located in _build_ directory then).
3. To get command line help: `smlp -h`
4. The `input_size` parameter (`-i` parameter) should be equal to the number of input data of your dataset and the `output_size` (`-o` parameter) to the output data of your dataset. input_size + output_size should be equal to your data fields number. If your output is at the beginning of your data rows, you can indicate it with the `output_ends` parameter (`-z` parameter).
5. `epochs` (`-e` parameter) will retrain the neural network with the dataset but after a backward propagation correction. It is better to set at least 100 epochs.
6. The `training_ratio` (`-t` parameter) is the ratio of the data file that SMLP will use for training. The remaining lines of the dataset will be used for testing. In other words, the first part of the file is used for training and the second part for testing. The training_ratio should be a valid ratio between 0 and 1, with a recommended value of 0.7.
7. The `learning_rate` (`-r` parameter) should be small enough to have a fine training but big enough to be efficient, a value of 0.01 is recommended with the mushroom example.
8. Increasing the hidden neurons per hidden layers `hidden_size` (`-d` parameter) and the hidden layers count `hiddens_count` (`-c` parameter) will make the neural network more complex and the training will go slower. Using just one hidden layer with 10 neurons is fine with a simple dataset like our mushroom example.
9. You can specify the running mode with the `mode` option (`-m` parameter), refer to the command line help for more details.

# Example

`smlp -f ../test/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100 -t 0.7 -r 0.01 -m TrainTestMonitored -v false`

Output:

```
...
[INFO] Training epoch 98/100... testing... acc(lah)[92 88 81] conv(01t)[96 97 96]
[INFO] Training epoch 99/100... testing... acc(lah)[92 88 81] conv(01t)[96 97 96]
[INFO] Training epoch 100/100... testing... acc(lah)[92 88 81] conv(01t)[96 97 96]
Elapsed time: 273.487s
Low accuracy (correct at 70%): 91.6%
Avg accuracy (correct at 80%): 88.2%
High accuracy (correct at 90%): 81.2%
Good convergence toward zero: 95.9% (21277/22177)
Good convergence toward one: 96.6%  (17215/17824)
Good convergence total: 96.2% (38492/40001)
```

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
