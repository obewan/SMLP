# SMLP

A Simple Multi-Layer Perceptron, by [Dams](https://dams-labs.net/).  
Each neurons of a previous layer is connected with each neurons of its next layer.

# Features

- Simplicity (the "S" of SMLP) for better understanding and for studying purpose, for improved maintainance and performance, and for compliance with best software development practices (low complexity, KISS principle). It can also be used as base for more complex projects.
- Functionality: operational, including forward propagation, backward propagation and weight update.
- Performance: uses C++ for optimal performance, and less CPU and memory usage.
- [Csv-parser](https://github.com/ashaduri/csv-parser) for CSV parsing. Dataset learning on the fly line by line (instead of loading the entire dataset into memory), resulting in low memory usage.
- [CLI11](https://github.com/CLIUtils/CLI11) for command line parsing.
- [JSON for Modern C++](https://github.com/nlohmann/json) for Json export/import.
- [Cmake](https://cmake.org) compilation, tested on Linux, but should be portable to other systems.
- [Doctest](https://github.com/doctest/doctest) for unit tests.
- [Gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) and [Lcov](https://github.com/linux-test-project/lcov) for code coverage, with 80% of minimum coverage validation.
- [Ctest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html) for integration tests.
- [Github Continuous Integration](https://docs.github.com/en/actions/automating-builds-and-tests/about-continuous-integration) with an unit test and a integration test checks.
- [Github CodeQL](https://github.com/features/security/code) security analysis.
- [Sonarlint](https://www.sonarsource.com/products/sonarlint) code analysis.
- [Doxygen](https://www.doxygen.nl) documentation.
- Includes a mushroom edibility dataset example.

# Roadmap to first release

- Add a more language files (i18n). `[in progress]`
- Add a pipe input.
- Add a socket input.
- Add an interactive testing (command line input).
- Add an auto-training feature (searching for the best parameters).
- Add a GUI (but probably on a more advanced project).

_Tensors, CUDA support and ONNX (Open Neural Network Exchange) format will be for an other and more advanced AI project._

# Usage

1. Prepare a dataset in CSV format with comma separation and with only float numbers ranging from 0 to 1 (example in data/mushroom we use a simple awk script to format mushroom_data_orig.csv into mushroom_data.csv).
2. Build and run smlp (should be located in _build_ directory then).
3. To get command line help: `smlp -h`
4. The `file_input` (`-f` parameter) must be the file path of your data file that will be used for training and testing.
5. The `import_network` option (`-i` parameter) is used to import an existing smlp network model for TestOnly and Predictive modes, instead of creating a new network model. It must be a model that has been previously exported with export_network.
6. The `export_network` option (`-e` parameter) allows you to export the trained model. This option can be used with the TrainOnly, TrainTestMonitored, and TrainThenTest modes.
7. The `input_size` (`-s` parameter) should be equal to the number of input data of your dataset and the `output_size` (`-o` parameter) to the output data of your dataset. input_size + output_size should be equal to your data fields number. If your output columns are at the ends of your dataset, you can indicate it with the `output_ends` (`-t` parameter).
8. `epochs` (`-p` parameter) will retrain the neural network with the dataset but after a backward propagation correction. It is better to set at least 100 epochs.
9. The `training_ratio` (`-r` parameter) is the ratio of the data file that SMLP will use for training. The remaining lines of the dataset will be used for testing. In other words, the first part of the file is used for training and the second part for testing. The training_ratio should be a valid ratio between 0 and 1, with a recommended value of 0.7.
10. The `learning_rate` (`-l` parameter) should be small enough to have a fine training but big enough to be efficient, a value of 0.01 is recommended with the mushroom example.
11. Increasing the hidden neurons per hidden layers `hidden_size` (`-d` parameter) and the hidden layers count `hiddens_count` (`-c` parameter) will make the neural network more complex and the training will go slower. Using just one hidden layer with 10 neurons is fine with a simple dataset like our mushroom example.
12. You can specify the running mode with the `mode` option (`-m` parameter):
    - Predictive: This mode uses an input file to predict the outputs. If the input file contains output columns, the predicted CSV outputs will replace them without modifying the original input file. Please be mindful of the parameters (input_size, output_size, output_ends). If the input file does not contain output columns, pay close attention to the input_size parameter. This mode requires a network model that has been imported and trained (be sure that the model has good testing results).
    - TestOnly: Test an imported network without training.
    - TrainOnly: Train the network without testing.
    - TrainThenTest: Train and then test the network (default mode).
    - TrainTestMonitored: Train and test at each epoch while monitoring the progress of an output neuron. Be aware that this is slower and uses more memory.
13. If using the Predictive mode, you can specify how to render the output with the `predictive_mode` option (`-n` parameter):
    - CSV: This will render the output(s) at the end or at the begining of the input line, depending of your `output_ends` option (default mode).
    - NumberAndRaw: This will show both the predicted output(s) numbers and their raw values (float).
    - NumberOnly: This will show only the predicted outputs number.
    - RawOnly: This will only show the output(s) raw values (float).
14. You can specify the activation function for hidden layers neurons with `hidden_activation_function` (`-a` parameter) and for the output layer with `output_activation_function` (`-b` parameter):
    - ELU: Exponential Linear Units, require an `hidden_activation_alpha` parameter (`-k` parameter) or an `output_activation_alpha` parameter (`-q` parameter).
    - LReLU: Leaky ReLU.
    - PReLU: Parametric ReLU, require an an `hidden_activation_alpha` parameter (`-k` parameter) or an `output_activation_alpha` parameter (`-q` parameter).
    - ReLU: Rectified Linear Unit.
    - Sigmoid (default).
    - Tanh: Hyperbolic Tangent.
15. You can also use a `smlp.conf` file in JSON format, located in the same directory as the smlp program. This way, you won’t have to use the corresponding parameters on the command line. Currently, it supports:
    - `lang_file`: The language file to use for the help. Currently available, by alphabetic order:
      - `i18n/de.json` (german)
      - `i18n/en.json` (english)
      - `i18n/es.json` (spanish)
      - `i18n/fr.json` (french)
      - `i18n/it.json` (italian)
      - `i18n/pt.json` (portuguese)
    - `file_input`: The dataset file to use, similar to the file_input option.
    - `import_network`: The model file to import, similar to the import_network option.
    - `export_network`: The model file to export, similar to the export_network option.

If you use the corresponding command line options, they will override the config file parameters. Here an example of `smlp.conf`:

```json
{
  "lang_file": "i18n/en.json",
  "file_input": "../data/mushroom/mushroom_data.csv",
  "import_network": "mushroom_model.json",
  "export_network": "mushroom_model_new.json"
}
```

# Examples

### Example 1: Training

Training a new MLP with the mushroom dataset, 20 inputs (-s), 1 output (-o), 12 neurons by hidden layers (-d), 1 hidden layer (-c), during 100 epochs (-p), output at beginning of the dataset (no -t), with a ReLU activation function on hidden layer (-a) and with monitored tests (-m). Then exporting the model to myMushroomMLP.json after the training (-e).

`smlp -e myMushroomMLP.json -f ../data/mushroom/mushroom_data.csv -s 20 -o 1 -d 12 -c 1 -p 100 -a ReLU -m TrainTestMonitored`

Output:

```
...
[2023-11-06 14:58:26] [INFO] Training epoch 93/100... testing... acc(lah)[87 85 82] conv(01t)[84 99 91]
[2023-11-06 14:58:28] [INFO] Training epoch 94/100... testing... acc(lah)[84 81 78] conv(01t)[78 1e+02 88]
[2023-11-06 14:58:30] [INFO] Training epoch 95/100... testing... acc(lah)[84 81 79] conv(01t)[78 1e+02 88]
[2023-11-06 14:58:41] [INFO] Training epoch 96/100... testing... acc(lah)[84 81 79] conv(01t)[78 1e+02 88]
[2023-11-06 14:58:43] [INFO] Training epoch 97/100... testing... acc(lah)[84 81 79] conv(01t)[78 1e+02 88]
[2023-11-06 14:58:45] [INFO] Training epoch 98/100... testing... acc(lah)[84 82 79] conv(01t)[79 99 88]
[2023-11-06 14:58:47] [INFO] Training epoch 99/100... testing... acc(lah)[84 82 79] conv(01t)[79 1e+02 88]
[2023-11-06 14:58:49] [INFO] Training epoch 100/100... testing... acc(lah)[85 83 80] conv(01t)[80 99 89]
[2023-11-06 14:58:51] [INFO] Elapsed time: 292.32s
[2023-11-06 14:58:51] [INFO] Testing results:
Low accuracy (correct at 70%): 84.8%
Avg accuracy (correct at 80%): 82.8%
High accuracy (correct at 90%): 79.6%
Good convergence toward zero: 80.1% (8183/10215)
Good convergence toward one: 99.4%  (8057/8106)
Good convergence total: 88.6% (16240/18321)

[2023-11-06 14:58:51] [INFO] Exporting network model to myMushroomMLP.json...
```

### Example 2: Testing

Importing the previous myMushroomMLP.json and testing it with the mushroom dataset.

`smlp -i myMushroomMLP.json -f ../data/mushroom/mushroom_data.csv -m TestOnly`

Output:

```
[2023-11-06 15:00:13] [INFO] Importing network model from myMushroomMLP.json...
[2023-11-06 15:00:13] [INFO] Testing, using file ../data/mushroom/mushroom_data.csv
Testing results:
Low accuracy (correct at 70%): 84.8%
Avg accuracy (correct at 80%): 82.8%
High accuracy (correct at 90%): 79.6%
```

### Example 3: Predict

Using a new data file that doesn't have outputs to predict the outputs with our previous mushroom mlp model.
`smlp -i myMushroomMLP.json -f ../data/mushroom/mushroom_data_to_predict.csv -m Predictive`

mushroom_data_to_predict.csv:

```
0.08,0.43,0.90,0.42,1.00,0.62,0.33,0.38,0.10,0.07,0.00,0.00,0.38,0.00,0.00,1.00,0.92,0.00,1.00,0.00
0.01,0.57,0.90,0.25,1.00,0.00,0.67,0.92,0.09,0.02,0.00,0.00,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00
0.07,0.57,0.00,0.25,1.00,0.38,0.33,0.62,0.11,0.07,0.14,0.00,0.08,0.00,0.00,1.00,0.92,0.00,0.25,0.00
0.09,0.57,0.00,0.67,1.00,0.62,0.33,0.62,0.13,0.20,0.00,0.00,0.62,0.00,0.00,1.00,0.92,0.00,0.38,0.00
0.16,0.71,0.40,0.08,1.00,0.38,0.33,0.69,0.12,0.20,0.00,0.64,0.08,0.00,0.00,1.00,0.92,0.00,1.00,0.00
0.06,0.14,0.40,1.00,1.00,0.50,0.00,0.69,0.17,0.06,0.00,0.00,0.92,0.00,0.00,1.00,0.92,0.00,0.12,0.00
0.02,0.43,0.20,0.08,1.00,0.38,0.67,0.08,0.04,0.02,0.00,0.36,0.08,0.00,0.00,1.00,0.92,0.00,0.62,0.00
0.09,1.00,0.70,0.75,1.00,0.00,0.33,0.85,0.03,0.09,0.00,0.00,0.08,0.00,0.00,1.00,0.92,0.00,1.00,0.00
0.07,0.71,0.00,0.08,0.50,0.38,0.33,0.69,0.10,0.06,0.00,0.00,0.54,0.00,0.00,1.00,0.92,0.00,1.00,0.00
0.06,0.43,0.00,0.75,1.00,0.12,0.33,0.62,0.11,0.09,0.00,0.00,0.23,0.00,0.00,1.00,0.92,0.00,1.00,0.00
```

Output:

```
1,0.08,0.43,0.9,0.42,1,0.62,0.33,0.38,0.1,0.07,0,0,0.38,0,0,1,0.92,0,1,0
1,0.01,0.57,0.9,0.25,1,0,0.67,0.92,0.09,0.02,0,0,0.62,0,0,1,0.92,0,1,0
0,0.07,0.57,0,0.25,1,0.38,0.33,0.62,0.11,0.07,0.14,0,0.08,0,0,1,0.92,0,0.25,0
1,0.09,0.57,0,0.67,1,0.62,0.33,0.62,0.13,0.2,0,0,0.62,0,0,1,0.92,0,0.38,0
1,0.16,0.71,0.4,0.08,1,0.38,0.33,0.69,0.12,0.2,0,0.64,0.08,0,0,1,0.92,0,1,0
0,0.06,0.14,0.4,1,1,0.5,0,0.69,0.17,0.06,0,0,0.92,0,0,1,0.92,0,0.12,0
0,0.02,0.43,0.2,0.08,1,0.38,0.67,0.08,0.04,0.02,0,0.36,0.08,0,0,1,0.92,0,0.62,0
1,0.09,1,0.7,0.75,1,0,0.33,0.85,0.03,0.09,0,0,0.08,0,0,1,0.92,0,1,0
0,0.07,0.71,0,0.08,0.5,0.38,0.33,0.69,0.1,0.06,0,0,0.54,0,0,1,0.92,0,1,0
1,0.06,0.43,0,0.75,1,0.12,0.33,0.62,0.11,0.09,0,0,0.23,0,0,1,0.92,0,1,0
```

The first column represents the predicted result: 1 signifies that the mushroom is likely edible, while 0 indicates it is likely not edible. These predictions are made with an accuracy of 80%, meaning that there is an 80% confidence in these predictions. However, caution is advised. While we can strive to improve the accuracy of the model, it’s important to remember that predictions are inherently probabilistic and will never reach 100% certainty, mirroring the inherent uncertainties of real life.

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
