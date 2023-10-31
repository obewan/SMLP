# SMLP

A Simple Multi-Layer Perceptron, by [Dams](https://dams-labs.net/).  
Each neurons of a previous layer is connected with each neurons of its next layer.

# Features

- Simplicity (the "S" of SMLP) for better understanding and for studying purpose, for improved maintainance and performance, and for compliance with best software development practices (low complexity, KISS principle). It can also be used as base for more complex projects.
- Functionality: operational, including forward propagation, backward propagation and weight update.
- Performance: uses C++ for optimal performance, and less CPU and memory usage.
- CSV parsing using [csv-parser](https://github.com/ashaduri/csv-parser), dataset learning on the fly line by line (instead of loading the entire dataset into memory), resulting in low memory usage.
- Command line parsing using [CLI11](https://github.com/CLIUtils/CLI11).
- Json export/import using [JSON for Modern C++](https://github.com/nlohmann/json).
- [Cmake](https://cmake.org) compilation, tested on Linux, but should be portable to other systems.
- [Doctest](https://github.com/doctest/doctest) unit tests.
- [Gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) and [Lcov](https://github.com/linux-test-project/lcov) code coverage.
- Github [Continuous Integration](https://docs.github.com/en/actions/automating-builds-and-tests/about-continuous-integration) with an unit test pass.
- Github [CodeQL](https://github.com/features/security/code) security analysis.
- [Sonarlint](https://www.sonarsource.com/products/sonarlint) code analysis.
- [Doxygen](https://www.doxygen.nl) documentation.
- Includes a mushroom edibility dataset example.

# Roadmap to first release

- Add a Predictive mode.`[in progress]`
- Add a pipe input.
- Add a config file.
- Improve code coverage to 80%.
- Add interactive testing.
- Add an auto-training feature (searching for the best parameters).
- Add a GUI (but probably on a more advanced project).

_Tensors, CUDA support and ONNX (Open Neural Network Exchange) format will be for an other and more advanced AI project._

# Usage

1. Prepare a dataset in CSV format with comma separation and with only float numbers ranging from 0 to 1 (example in data/mushroom we use a simple awk script to format mushroom_data_orig.csv into mushroom_data.csv).
2. Build and run smlp (should be located in _build_ directory then).
3. To get command line help: `smlp -h`
4. The `file_input` (`-f` parameter) must be the file path of your data file that will be used for training and testing.
5. The `input_size` (`-i` parameter) should be equal to the number of input data of your dataset and the `output_size` (`-o` parameter) to the output data of your dataset. input_size + output_size should be equal to your data fields number. If your output is at the beginning of your data rows, you can indicate it with the `output_ends` (`-s` parameter).
6. `epochs` (`-e` parameter) will retrain the neural network with the dataset but after a backward propagation correction. It is better to set at least 100 epochs.
7. The `training_ratio` (`-t` parameter) is the ratio of the data file that SMLP will use for training. The remaining lines of the dataset will be used for testing. In other words, the first part of the file is used for training and the second part for testing. The training_ratio should be a valid ratio between 0 and 1, with a recommended value of 0.7.
8. The `learning_rate` (`-r` parameter) should be small enough to have a fine training but big enough to be efficient, a value of 0.01 is recommended with the mushroom example.
9. Increasing the hidden neurons per hidden layers `hidden_size` (`-d` parameter) and the hidden layers count `hiddens_count` (`-c` parameter) will make the neural network more complex and the training will go slower. Using just one hidden layer with 10 neurons is fine with a simple dataset like our mushroom example.
10. You can specify the running mode with the `mode` option (`-m` parameter):
    - TestOnly: Just test an imported network without training.
    - TrainOnly: Just train the network without testing.
    - TrainThenTest: Train at once then test (default mode).
    - TrainTestMonitored: Train and test at each epoch with monitoring progress of an output neuron. Beware as this is slower and uses more memory.
11. You can specify the activation function for hidden layers neurons with `hidden_activation_function` (`-j` parameter) and for the output layer with `output_activation_function` (`-k` parameter):
    - ELU: Exponential Linear Units, require an hidden_activation_alpha parameter (`-p` parameter) or an output_activation_alpha parameter (`-q` parameter).
    - LReLU: Leaky ReLU.
    - PReLU: Parametric ReLU, require an an hidden_activation_alpha parameter (`-p` parameter) or an output_activation_alpha parameter (`-q` parameter).
    - ReLU: Rectified Linear Unit.
    - Sigmoid (default).
    - Tanh: Hyperbolic Tangent.

# Examples

### Example 1:

Training a new MLP with the mushroom dataset, 20 inputs, 1 output, 12 neurons by hidden layers, 1 hidden layer, during 100 epochs, with 70% training data and 30% testing data, a learning rate of 0.01 and with monitored tests. Then exporting the model to myMushroomMLP.json after the training.

`smlp -b myMushroomMLP.json -f ../data/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100 -t 0.7 -r 0.01 -m TrainTestMonitored`

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
[INFO] Exporting network model to myMushroomMLP.json...
```

### Example 2:

Importing the previous myMushroomMLP.json and testing it with the mushroom dataset.

`smlp -a myMushroomMLP.json -f ../data/mushroom/mushroom_data.csv -m TestOnly`

Output:

```
[INFO] Importing network model from myMushroomMLP.json...
Testing, using file ../data/mushroom/mushroom_data.csv
Low accuracy (correct at 70%): 91.6%
Avg accuracy (correct at 80%): 88.2%
High accuracy (correct at 90%): 81.2%
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
