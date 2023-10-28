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

bool SimpleMLP::init(int argc, char **argv,
                     [[maybe_unused]] bool withMonitoring) {
  try {
    if (parseArgs(argc, argv) != EXIT_SUCCESS || params.data_file.empty()) {
      return false;
    }

    network_ = new Network(params);

    return true;

  } catch (std::exception &ex) {
    std::cerr << "[ERROR] " << ex.what() << std::endl;
    return false;
  }
}

void SimpleMLP::train() {
  std::cout << "Training, using file " << params.data_file << std::endl;
  std::cout << "InputSize:" << params.input_size
            << " OutputSize:" << params.output_size
            << " HiddenSize:" << params.hidden_size
            << " HiddenLayers:" << params.hiddens_count
            << " Epochs:" << params.num_epochs
            << " TrainingRatio:" << params.training_ratio
            << " LearningRate:" << params.learning_rate
            << " Mode:" << Common::getModeStr(params.mode)
            << " Verbose:" << params.verbose << std::endl;

  Training training(network_, params.data_file);
  training.train(params);
}

void SimpleMLP::test() {
  std::cout << "Testing, using file " << params.data_file << std::endl;
  Testing testing(network_, params.data_file);
  testing.test(params, 0);
  testing.showResults();
}

void SimpleMLP::trainTestMonitored() {
  if (params.output_index_to_monitor > params.output_size) {
    std::cerr << "[ERROR] output_index_to_monitor > output_size: "
              << params.output_index_to_monitor << ">" << params.output_size
              << std::endl;
    return;
  }

  std::cout << "Train and testing, using file " << params.data_file
            << std::endl;
  std::cout << "InputSize:" << params.input_size
            << " OutputSize:" << params.output_size
            << " HiddenSize:" << params.hidden_size
            << " HiddenLayers:" << params.hiddens_count
            << " Epochs:" << params.num_epochs
            << " TrainingRatio:" << params.training_ratio
            << " LearningRate:" << params.learning_rate
            << " Mode:" << Common::getModeStr(params.mode)
            << " OutputIndexToMonitor:" << params.output_index_to_monitor
            << " Verbose:" << params.verbose << std::endl;

  Training training(network_, params.data_file);
  training.trainTestMonitored(params);
}

int SimpleMLP::parseArgs(int argc, char **argv) {

  CLI::App app{params.title};

  app.add_option("-f,--file_input", params.data_file,
                 "the data file to use for training and testing")
      ->mandatory()
      ->check(CLI::ExistingPath);
  app.add_option("-i,--input_size", params.input_size,
                 "the numbers of input neurons")
      ->mandatory()
      ->check(CLI::PositiveNumber);
  app.add_option("-o,--output_size", params.output_size,
                 "the numbers of output neurons")
      ->default_val(params.output_size)
      ->check(CLI::PositiveNumber);
  app.add_option("-d,--hidden_size", params.hidden_size,
                 "the numbers of hidden neurons per hidden layer")
      ->default_val(params.hidden_size)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-c,--hiddens_count", params.hiddens_count,
                 "the count of hidden layers")
      ->default_val(params.hiddens_count)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-e,--epochs", params.num_epochs,
                 "the numbers of epochs retraining")
      ->default_val(params.num_epochs)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-r,--learning_rate", params.learning_rate,
                 "optimizer learning rate")
      ->default_val(params.learning_rate)
      ->check(CLI::PositiveNumber)
      ->check(CLI::TypeValidator<float>());
  app.add_option("-t,--training_ratio", params.training_ratio,
                 "the training ratio of the file to switch between data for "
                 "training and data for testing, should be around 0.7.")
      ->default_val(params.training_ratio)
      ->check(CLI::Range(0.0f, 1.0f))
      ->check(CLI::TypeValidator<float>());
  app.add_option(
         "-z,--output_ends", params.output_at_end,
         "indicate if the output data is at the end of the record (1) or at "
         "the beginning (0)")
      ->default_val(params.output_at_end)
      ->check(CLI::TypeValidator<bool>());
  app.add_option(
         "-m, --mode", params.mode,
         "Select the running mode:\n"
         "  - TrainOnly: Just train the network without testing.\n"
         "  - TestOnly: Just test an imported network without training.\n"
         "  - TrainThenTest: Train at once then test (default mode).\n"
         "  - TrainTestMonitored: Train and test at each epoch with monitoring "
         "progress of an output neuron. Beware as this is slower and uses more "
         "memory.")
      ->transform(CLI::CheckedTransformer(mode_map, CLI::ignore_case));
  app.add_option("-y, --output_index_to_monitor",
                 params.output_index_to_monitor,
                 "indicate the output neuron index to monitor during a "
                 "TrainTestMonitored mode. If index = 0 there will be no "
                 "progress monitoring. Default is 1, the first neuron output.")
      ->default_val(params.output_index_to_monitor)
      ->check(CLI ::NonNegativeNumber);
  app.add_option("-v,--verbose", params.verbose, "verbose logs")
      ->default_val(params.verbose)
      ->check(CLI::TypeValidator<bool>());

  CLI11_PARSE(app, argc, argv)
  return EXIT_SUCCESS;
}
