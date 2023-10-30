/**
 * \mainpage SMLP
 * A Simple Multi-Layer Perceptron to study artificial networks.
 * \author  Damien Balima (https://dams-labs.net)
 * \date    August 2023
 * \copyright
 *
 * [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International
License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]:
https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg

- *Attribution*: you must give appropriate credit, provide a link to the
license, and indicate if changes were made. You may do so in any reasonable
manner, but not in any way that suggests the licensor endorses you or your use.
- *NonCommercial*: you may not use the material for commercial purposes.
- *ShareAlike*: if you remix, transform, or build upon the material, you must
distribute your contributions under the same license as the original.
- *No additional restrictions*: you may not apply legal terms or technological
measures that legally restrict others from doing anything the license permits.
 */

#include "include/SimpleMLP.h"
#include "Network.h"
#include "Testing.h"
#include "Training.h"
#include "include/CLI11.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

bool SimpleMLP::init(int argc, char **argv, bool &showVersion) {
  try {

    if (parseArgs(argc, argv, showVersion) != EXIT_SUCCESS ||
        network_params.data_file
            .empty()) { // in case of show help or show version
      return false;
    }

    network_ = new Network(network_params);

    return true;

  } catch (std::exception &ex) {
    std::cerr << "[ERROR] " << ex.what() << std::endl;
    return false;
  }
}

void SimpleMLP::train() {
  std::cout << "Training, using file " << network_params.data_file << std::endl;
  std::cout << "InputSize:" << network_params.input_size
            << " OutputSize:" << network_params.output_size
            << " HiddenSize:" << network_params.hidden_size
            << " HiddenLayers:" << network_params.hiddens_count
            << " Epochs:" << network_params.num_epochs
            << " TrainingRatio:" << network_params.training_ratio
            << " LearningRate:" << network_params.learning_rate
            << " Mode:" << Common::getModeStr(network_params.mode)
            << " Verbose:" << network_params.verbose << std::endl;

  Training training(network_, network_params.data_file);
  training.train(network_params);
}

void SimpleMLP::test() {
  std::cout << "Testing, using file " << network_params.data_file << std::endl;
  Testing testing(network_, network_params.data_file);
  testing.test(network_params, 0);
  testing.showResults();
}

void SimpleMLP::trainTestMonitored() {
  if (network_params.output_index_to_monitor > network_params.output_size) {
    std::cerr << "[ERROR] output_index_to_monitor > output_size: "
              << network_params.output_index_to_monitor << ">"
              << network_params.output_size << std::endl;
    return;
  }

  std::cout << "Train and testing, using file " << network_params.data_file
            << std::endl;
  std::cout << "InputSize:" << network_params.input_size
            << " OutputSize:" << network_params.output_size
            << " HiddenSize:" << network_params.hidden_size
            << " HiddenLayers:" << network_params.hiddens_count
            << " Epochs:" << network_params.num_epochs
            << " TrainingRatio:" << network_params.training_ratio
            << " LearningRate:" << network_params.learning_rate
            << " Mode:" << Common::getModeStr(network_params.mode)
            << " OutputIndexToMonitor:"
            << network_params.output_index_to_monitor
            << " Verbose:" << network_params.verbose << std::endl;

  Training training(network_, network_params.data_file);
  training.trainTestMonitored(network_params);
}

int SimpleMLP::parseArgs(int argc, char **argv, bool &showVersion) {

  CLI::App app{app_params.title};

  app.add_option("-f,--file_input", network_params.data_file,
                 "the data file to use for training and testing")
      ->check(CLI::ExistingPath);
  app.add_option("-i,--input_size", network_params.input_size,
                 "the numbers of input neurons")
      ->check(CLI::PositiveNumber);
  app.add_option("-o,--output_size", network_params.output_size,
                 "the numbers of output neurons")
      ->default_val(network_params.output_size)
      ->check(CLI::PositiveNumber);
  app.add_option("-d,--hidden_size", network_params.hidden_size,
                 "the numbers of hidden neurons per hidden layer")
      ->default_val(network_params.hidden_size)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-c,--hiddens_count", network_params.hiddens_count,
                 "the count of hidden layers")
      ->default_val(network_params.hiddens_count)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-e,--epochs", network_params.num_epochs,
                 "the numbers of epochs retraining")
      ->default_val(network_params.num_epochs)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-r,--learning_rate", network_params.learning_rate,
                 "optimizer learning rate")
      ->default_val(network_params.learning_rate)
      ->check(CLI::PositiveNumber)
      ->check(CLI::TypeValidator<float>());
  app.add_option(
         "-s,--output_ends", network_params.output_at_end,
         "indicate if the output data is at the end of the record (1) or at "
         "the beginning (0)")
      ->default_val(network_params.output_at_end)
      ->check(CLI::TypeValidator<bool>());
  app.add_option("-t,--training_ratio", network_params.training_ratio,
                 "the training ratio of the file to switch between data for "
                 "training and data for testing, should be around 0.7.")
      ->default_val(network_params.training_ratio)
      ->check(CLI::Range(0.0f, 1.0f))
      ->check(CLI::TypeValidator<float>());
  app.add_option(
         "-m, --mode", network_params.mode,
         "Select the running mode:\n"
         "  - TestOnly: Just test an imported network without training.\n"
         "  - TrainOnly: Just train the network without testing.\n"
         "  - TrainThenTest: Train at once then test (default mode).\n"
         "  - TrainTestMonitored: Train and test at each epoch with monitoring "
         "progress of an output neuron. Beware as this is slower and uses more "
         "memory.")
      ->transform(CLI::CheckedTransformer(mode_map, CLI::ignore_case));
  app.add_option("-y, --output_index_to_monitor",
                 network_params.output_index_to_monitor,
                 "indicate the output neuron index to monitor during a "
                 "TrainTestMonitored mode. If index = 0 there will be no "
                 "progress monitoring. Default is 1, the first neuron output.")
      ->default_val(network_params.output_index_to_monitor)
      ->check(CLI ::NonNegativeNumber);
  app.add_flag("-v,--version", showVersion, "show current version");
  app.add_flag("-w,--verbose", network_params.verbose, "verbose logs");

  CLI11_PARSE(app, argc, argv) return EXIT_SUCCESS;
}
