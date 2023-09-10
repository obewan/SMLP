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
#include "AdamOptimizer.h"
#include "Monitor.h"
#include "Network.h"
#include "Testing.h"
#include "Training.h"
#include "include/CLI11.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments: 1:input_size 2:hidden_size
 * @code {.bash}
 * smlp -f ../test/mushroom/mushroom_data.csv -i 20 -s 20 -o 1 -e 10
 * -l 40000 -z false
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  // Parsing arguments
  Parameters params = {.title = "SMLP",
                       .data_file = "",
                       .input_size = 0,
                       .hidden_size = 10,
                       .output_size = 1,
                       .num_epochs = 3,
                       .to_line = 0,
                       .learning_rate = 1e-3f,
                       .beta1 = 0.9f,
                       .beta2 = 0.99f,
                       .output_at_end = false};

  parseArgs(argc, argv, params);

  // Create instances of Network, Optimizer, and TrainingData
  Optimizer *optimizer =
      new AdamOptimizer(params.learning_rate, params.beta1, params.beta2);

  auto network =
      new Network(params.input_size, params.hidden_size, params.output_size,
                  optimizer, params.learning_rate);
  auto monitor = buildMonitor(network);
  network->SetMonitor(monitor);

  std::cout << "Training..." << std::endl;
  Training training(network, params.data_file, optimizer);
  if (!training.Train(params.num_epochs, params.output_at_end, 0,
                      params.to_line)) {
    std::cerr << "[ERROR] Training error. Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Testing..." << std::endl;
  Testing testing(network, params.data_file);
  testing.Test(params.output_at_end, params.to_line, 0);

  return 0;
}

int parseArgs(int argc, char *argv[], Parameters &params) {

  CLI::App app{params.title};

  app.add_option("-f,--file_input", params.data_file,
                 "the data file to use for training and testing")
      ->mandatory()
      ->check(CLI::ExistingPath);
  app.add_option("-i,--input_size", params.input_size,
                 "the numbers of input neurons")
      ->mandatory()
      ->check(CLI::PositiveNumber);
  app.add_option("-s,--hidden_size", params.hidden_size,
                 "the numbers of hidden neurons")
      ->default_val(params.hidden_size)
      ->check(CLI::PositiveNumber);
  app.add_option("-o,--output_size", params.output_size,
                 "the numbers of output neurons")
      ->default_val(params.output_size)
      ->check(CLI::PositiveNumber);
  app.add_option("-e,--epochs", params.num_epochs,
                 "the numbers of epochs retraining")
      ->default_val(params.num_epochs)
      ->check(CLI::PositiveNumber);
  app.add_option(
         "-l,--line_to", params.to_line,
         "the line number until the training will complete and testing will "
         "start, or 0 to use the entire file")
      ->default_val(params.to_line)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-w,--learning_rate", params.learning_rate,
                 "optimizer learning rate")
      ->default_val(params.learning_rate)
      ->check(CLI::TypeValidator<float>());
  app.add_option("-x,--beta1", params.beta1, "optimizer beta1")
      ->default_val(params.beta1)
      ->check(CLI::TypeValidator<float>());
  app.add_option("-y,--beta2", params.beta2, "optimizer beta2")
      ->default_val(params.beta2)
      ->check(CLI::TypeValidator<float>());
  app.add_option(
         "-z,--output_ends", params.output_at_end,
         "indicate if the output data is at the end of the record (1) or at "
         "the beginning (0)")
      ->default_val(params.output_at_end)
      ->check(CLI::TypeValidator<bool>());

  CLI11_PARSE(app, argc, argv)
  return EXIT_SUCCESS;
}

Monitor *buildMonitor(Network *network) {
  // Get timestamp for monitor file name
  auto time = std::time(nullptr);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time),
                      "%F_%T"); // ISO 8601 without timezone information.
  auto dateTime = ss.str();
  std::erase(dateTime, ':');
  std::erase(dateTime, '-');
  ss.str("");
  ss.clear();
  std::string filename = "monitor_" + dateTime + ".csv";
  auto *monitor = new Monitor(filename);
  std::cout << "Monitoring to file " << filename << std::endl;

  // Add header line to monitor file
  monitor->log("date");
  for (auto const &con : network->GetInputLayer()->Connections()) {
    ss << "ic" << con.GetSourceUnit() << "-" << con.GetDestinationUnit();
    monitor->log(ss.str());
    ss.str("");
    ss.clear();
  }
  size_t hs = 0;
  for (auto const &hidden : network->GetHiddenLayers()) {
    hs++;
    for (auto const &con : hidden->Connections()) {
      ss << "h" << hs << "c" << con.GetSourceUnit() << "-"
         << con.GetDestinationUnit();
      monitor->log(ss.str());
      ss.str("");
      ss.clear();
    }
  }
  size_t output_size = network->GetOutputSize();
  for (size_t i = 0; i < output_size; i++) {
    ss << "op" << i + 1;
    monitor->log(ss.str());
    ss.str("");
    ss.clear();
  }
  for (size_t i = 0; i < output_size; i++) {
    ss << "oe" << i + 1;
    monitor->log(ss.str(), i == output_size - 1);
    ss.str("");
    ss.clear();
  }

  return monitor;
}