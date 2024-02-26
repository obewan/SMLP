# SMLP

> "Simplicity is the ultimate sophistication" _- Leonardo da Vinci (attributed)_

A Simple Multi-Layer Perceptron, by [Dams](https://dams-labs.net/).  
Each neurons of a previous layer is connected with each neurons of its next layer.

# Features

- Simplicity (the "S" of SMLP) for better understanding and for studying purpose, for improved maintainance and performance, and for compliance with best software development practices (low complexity, KISS principle). It can also be used as base for more complex projects.
- Functionality: operational, including forward propagation, backward propagation and weight update.
- Performance: uses C++ for optimal performance, and less CPU and memory usage.

**Including:**

- [Csv-parser](https://github.com/ashaduri/csv-parser) for CSV parsing. Dataset learning on the fly line by line (instead of loading the entire dataset into memory), resulting in low memory usage.
- [CLI11](https://github.com/CLIUtils/CLI11) for command line parsing.
- [JSON for Modern C++](https://github.com/nlohmann/json) for Json export/import.
- [Cmake](https://cmake.org) compilation, tested on Linux, but should be portable to other systems.
- [Doctest](https://github.com/doctest/doctest) unit tests.
- [Ctest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html) integration tests.
- [Gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) and [Lcov](https://github.com/linux-test-project/lcov) code coverage, with 80% of minimum coverage validation.
- [Doxygen](https://www.doxygen.nl) documentation.
- [Github Continuous Integration](https://docs.github.com/en/actions/automating-builds-and-tests/about-continuous-integration) with an unit test and a integration test checks.
- [Github CodeQL](https://github.com/features/security/code) security analysis.

**Also used:**

- [Sonarlint](https://www.sonarsource.com/products/sonarlint) code analysis.
- [CppCheck](https://cppcheck.sourceforge.io/) code analysis.
- [CCCC](https://sarnold.github.io/cccc/CCCC_User_Guide.html) code metrics.
- [Valgrind](https://valgrind.org/) memory check.
- [Microsoft Copilot](https://www.microsoft.com/en-us/bing?ep=140&es=31&form=MA13FV), [OpenAI ChatGPT](https://chat.openai.com/) and [Google Gemini](https://gemini.google.com/) code reviews.

Includes a mushroom edibility dataset example.

# Roadmap to the first release

- Preparing the release (cleaning, refactoring, testing, documentation, packaging). First release comming soon... `[in progress]`

# Usage

1. Prepare a dataset in CSV format with comma separation and with only float numbers ranging from 0 to 1 (example in data/mushroom we use a simple awk script to format mushroom_data_orig.csv into mushroom_data.csv).
2. Build and run smlp (should be located in _build_ directory then).
3. To get command line help: `smlp -h`
4. You can specify the running mode with the `mode` option (`-m` parameter):
   - Predict: This mode uses an input file to predict the outputs. If the input file contains output columns, the predicted CSV outputs will replace them without modifying the original input file. Please be mindful of the parameters (input_size, output_size, output_ends). If the input file does not contain output columns, pay close attention to the input_size parameter. This mode requires a network model that has been imported and trained (be sure that the model has good testing results).
   - TestOnly: Test an imported network without training.
   - TrainOnly: Train the network without testing.
   - TrainThenTest: Train and then test the network (default mode).
   - TrainTestMonitored: Train and test at each epoch while monitoring the progress of an output neuron. Be aware that this is slower and uses more memory.
5. You can also use a `smlp.conf` file in JSON format, located in the same directory as the smlp program. This way, you won’t have to use the corresponding parameters on the command line. Currently, it supports:
   - `lang_file`: The language file to use, for the help and other messages. Currently available, by alphabetic order:
     - `i18n/de.json` (german)
     - `i18n/en.json` (english - default)
     - `i18n/es.json` (spanish)
     - `i18n/fr.json` (french)
     - `i18n/it.json` (italian)
     - `i18n/pt.json` (portuguese)
   - `file_input`: The dataset file to use, similar to the file_input option.
   - `import_network`: The JSON model file to import, similar to the import_network option. Ensure that the corresponding CSV file is available.
   - `export_network`: The JSON model file to export, similar to the export_network option. Executing this will also create or update the corresponding CSV file.

If you use the corresponding command line options, they will override the config file parameters. Here an example of `smlp.conf`:

```json
{
  "lang_file": "i18n/en.json",
  "file_input": "../data/mushroom/mushroom_data.csv",
  "import_network": "mushroom_model.json",
  "export_network": "mushroom_model_new.json"
}
```

<details>
<summary>Command help (click to expand)</summary>

```bash
$ ./smlp -h
SMLP - Simple Multilayer Perceptron
Usage: ./smlp [OPTIONS]

Options:
-h,--help Print this help message and exit
-i,--import_network TEXT:FILE
Import a network model instead of creating a new one. This must be a valid model filepath, specifically a file generated by SMLP. If this option is used, there is no need to specify layer parameters as they are included in the model.
-e,--export_network TEXT Export the network model after training. This must be a valid filepath. The exported model can be imported later, eliminating the need for retraining.
-f,--file_input TEXT:FILE Specify the data file to be used for training and testing.
-s,--input_size UINT:POSITIVE [0]
The numbers of input neurons
-o,--output_size UINT:POSITIVE [1]
The numbers of output neurons
-d,--hidden_size UINT:NONNEGATIVE [10]
The numbers of hidden neurons per hidden layer
-c,--hiddens_count UINT:NONNEGATIVE [1]
The count of hidden layers
-p,--epochs UINT:NONNEGATIVE [3]
The numbers of epochs retraining
-l,--learning_rate FLOAT:FLOAT in [0 - 1]:FLOAT [0.01]
The network training learning rate
-t,--output_ends Indicate that the output columns of the dataset are located at the end of the record.
By default smlp will look at the firsts columns
-r,--training_ratio FLOAT:FLOAT in [0 - 1]:FLOAT [0.7]
The training ratio of the file to switch between data for training and data for testing, should be around 0.7
-R,--training_ratio_line UINT:NONNEGATIVE [0]
The training ratio line number of the file to switch between data for training and data for testing, should be located at 70% of the file.
-m,--mode ENUM:value in {Predict->0,TestOnly->1,TrainOnly->2,TrainTestMonitored->3,TrainThenTest->4} OR {0,1,2,3,4} [4]
Select the running mode: - Predict:This mode uses an input file to predict the outputs.
If the input file contains output columns, the predicted CSV outputs will replace them without modifying the original input file.
Please be mindful of the parameters (input_size, output_size, output_ends). If the input file does not contain output columns, pay close attention to the input_size parameter.
This mode requires a network that has been imported and trained (be sure that the model has good testing results). - TestOnly: Test an imported network without training. - TrainOnly: Train the network without testing. - TrainThenTest: Train and then test the network (default). - TrainTestMonitored: Train and test at each epoch while monitoring the progress of an output neuron. Be aware that this is slower and uses more memory.
-n,--predict_mode ENUM:value in {CSV->0,NumberAndRaw->1,NumberOnly->2,RawOnly->3} OR {0,1,2,3} [0]
If using Predict mode, select the output render mode: - CSV: This will render the output(s) at the end or at the begining of the input line, depending of your output_ends option (default). - NumberAndRaw: This will show both the predicted output(s) numbers and their raw values. - NumberOnly: This will show only the predicted outputs number. - RawOnly: This will only show the output(s) raw values.
-y,--output_index_to_monitor UINT:NONNEGATIVE [1]
Indicate the output neuron index to monitor during a TrainTestMonitored mode. If index = 0 there will be no progress monitoring. Default is 1, the first neuron output.
-a,--hidden_activation_function ENUM:value in {ELU->0,LReLU->1,PReLU->2,ReLU->3,Sigmoid->4,Tanh->5} OR {0,1,2,3,4,5} [4]
Select the hidden neurons activation function: - ELU: Exponential Linear Units, require an hidden_activation_alpha parameter. - LReLU: Leaky ReLU. - PReLU: Parametric ReLU, require an hidden_activation_alpha_parameter. - ReLU: Rectified Linear Unit. - Sigmoid (default). - Tanh: Hyperbolic Tangent
-b,--output_activation_function ENUM:value in {ELU->0,LReLU->1,PReLU->2,ReLU->3,Sigmoid->4,Tanh->5} OR {0,1,2,3,4,5} [4]
Select the output neurons activation function: - ELU: Exponential Linear Units, require an output_activation_alpha parameter. - LReLU: Leaky ReLU. - PReLU: Parametric ReLU, require an output_activation_alpha parameter. - ReLU: Rectified Linear Unit. - Sigmoid (default). - Tanh: Hyperbolic Tangent
-A,--hidden_activation_alpha FLOAT:FLOAT in [-100 - 100] [0.1]
The alpha parameter value for ELU and PReLU activation functions on hidden layers
-B,--output_activation_alpha FLOAT:FLOAT in [-100 - 100] [0.1]
The alpha parameter value for ELU and PReLU activation functions on output layer
-H,--enable_http Enable the HTTP service
-P,--http_port UINT:NONNEGATIVE [8080]
Set the HTTP service port
-x,--disable_stdin Disable stdin input like command pipes and interactive testing
-v,--version Show current version
-V,--verbose Verbose logs

```

</details>

&nbsp;

# Examples

<details>
<summary>Example 1: training</summary>

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
Good convergence toward one: 99.4% (8057/8106)
Good convergence total: 88.6% (16240/18321)

[2023-11-06 14:58:51] [INFO] Exporting network model to myMushroomMLP.json...

```

</details>

<details>
<summary>Example 2: testing</summary>

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

</details>

<details>
<summary>Example 3: predict</summary>

Using a new data file that doesn't have outputs to predict the outputs with our previous mushroom mlp model.
`smlp -i myMushroomMLP.json -f ../data/mushroom/mushroom_data_to_predict.csv -m Predict`

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

</details>

<details>
<summary>Example 4: using pipes</summary>

You can chain commands with pipes. This allows you to, for example, have a parser command before predicting the data with a neural network, and then chain the output with another command. Please note that when using pipes, you should use the `-R,--training_ratio_line` option instead of `training_ratio`. Also, be aware that there’s no `epoch` option available with pipes. However, you can use a loop in a script for that purpose.
Here some examples:

```bash
# create a new network model
$ cat ../data/mushroom/mushroom_data.csv | ./smlp -e mushroom_model.json -s 20 -o 1 -d 12 -c 1 -R 40000 -a ReLU -m TrainTestMonitored
[2023-11-17 20:56:16] [INFO] Training...testing... acc(lah)[30 13 5.1]

# train the network model again, the model accuracy (low, average, high) increase
$ cat ../data/mushroom/mushroom_data.csv | ./smlp -i mushroom_model.json -e mushroom_model.json -R 40000 -m TrainTestMonitored
[2023-11-17 21:00:14] [INFO] Training...testing... acc(lah)[52 36 20]

# test the model with some data (here the last 1000 lines) that shouldn't have been used during the training:
$ tail -n 1000 ../data/mushroom/mushroom_data.csv | ./smlp -i mushroom_model.json -R 40000 -m TestOnly

# train then test the model (no monitored):
$ cat ../data/mushroom/mushroom_data.csv | ./smlp -i mushroom_model.json -e mushroom_model.json -R 40000 -m TrainThenTest

# just train the model with the first 40000 lines (for the whole file, don't use the -R parameter), no test:
$ cat ../data/mushroom/mushroom_data.csv | ./smlp -i mushroom_model.json -e mushroom_model.json -R 40000 -m TrainOnly

# do some prediction then chain the output with another command, for example here to train a new model.
$ cat ../data/mushroom/mushroom_data_to_predict.csv | ./smlp -i mushroom_model.json -m Predict | ./smlp -e new_model.json -s 20 -o 1 -d 12 -c 1 -R 40000 -a ReLU -m TrainTestMonitored
```

</details>

<details>
<summary>Example 5: using http</summary>

To enable the HTTP service of SMLP, follow these steps:

Start SMLP on the server with the HTTP service enabled, for example on port 8080:

```bash
./smlp -e "myMushroomMLP.json" -i "myMushroomMLP.json" -V -H -P 8080
```

On the client side, send HTTP requests using the POST method. Make sure to use a path that corresponds to a supported SMLP mode (such as predict, trainonly, testonly, trainthentest, or traintestmonitored). The data result for the client will be in JSON format in the HTTP response body.
Here’s an example using curl:

```bash
response=$(curl -s -X POST -H "Content-Type: text/plain" -d '1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/testonly)
echo "$response"
```

Please note that this is a basic HTTP service designed for development purposes. Exercise caution while using it. It does not include SSL, so if you’re using it on the Internet or require security, ensure it’s used under an SSL layer (for example, using Apache’s SSL module).

You can also explore additional HTTP examples in the [./test/httpTests.sh](./test/httpTests.sh) script.

</details>

<details>
<summary>Example 6: using Docker</summary>

Be sure to have installed a `Docker` solution, like `Docker Desktop` (and activate its WSL support if using WSL). Then edit the `Dockerfile` to change the `MODEL_NAME` environment variable to the path of the smlp neural network model you want to use. This model should be located in the same directory as your Dockerfile.

Run the following command to build the Docker image (replace smlp with the name you want to give to your Docker image):

`docker build -t smlp .`

This command tells Docker to build an image using the Dockerfile in the current directory (.) and tag (-t) it with the name smlp.

Then run the Docker image on your local port 8080 (or any other port you prefer):

`docker run -p 8080:8080 smlp`

This command tells Docker to run a new container from the smlp image and map port 8080 of the container to port 8080 of your host machine.

Then you can send some request to the smlp server, like for example using curl:

`curl -s -X POST -H "Content-Type: text/plain" -d '0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00' http://localhost:8080/predict`

You should see a response like this:

`{"action":"Predict","code":0,"data":"1,0.04,0.57,0.8,0.08,1,0.38,0,0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0","message":"Success"}`

Note: If you have a service running on port 8080, you will need to choose a different port. You can also set additional smlp options in the Dockerfile, at the `ENV CMD` bottom line.

</details>

&nbsp;

# Notes for Developers

## Building from sources

<details>
<summary>On Linux</summary>

Ensure that `make`, `cmake`, `g++` (v10 or v11), and `lcov` are installed. For example, on Debian, you can use the following commands:  
`sudo apt-get update`  
`sudo apt-get install -y make cmake g++-11 lcov`

Next, configure CMake for the Release:  
`mkdir build`  
`CC=gcc-11 CXX=g++-11 cmake -B build -DCMAKE_BUILD_TYPE=Release`

Finally, build the project, you should find the `smlp` program under the `build` folder (don't forget to copy the `i18n` directory with it):  
`cmake --build build --config Release -- -j 2`

</details>

<details>
<summary>On Windows</summary>

Ensure that `cmake` for Windows and `Visual Studio` (which include a C++ compiler) are installed.

Then generate the build files:  
`mkdir build`  
`cd build`
`cmake -G "Visual Studio 17 2022" -A x64 ..`  
(Note: "Visual Studio 17 2022" refers to the 2022 community edition)

Make sure that exceptions are enabled in the `tester` project. This is necessary because some unit tests check for exceptions. You can enable exceptions by navigating to `Properties > C/C++ > Code Generation > Enable C++ Exceptions`.

Finally build the project, you should find the `smlp` program under the `build\Release` folder (don't forget to copy the `i18n` directory with it):  
`cmake --build . --config Release`

</details>

&nbsp;

---

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg

&nbsp;  
[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]
[![API](https://img.shields.io/badge/API-Documentation-blue)](https://obewan.github.io/SMLP/api/)
[![Coverage](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fobewan.github.io%2FSMLP%2Fcoverage%2Fcoverage.json&query=coverage&label=coverage&color=green)](https://obewan.github.io/SMLP/coverage/html/)

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]
