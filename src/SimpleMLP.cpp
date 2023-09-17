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

bool SimpleMLP::init(int argc, char **argv, bool withMonitoring) {
  try {
    if (parseArgs(argc, argv) != EXIT_SUCCESS) {
      return false;
    }

    optimizer_ =
        new AdamOptimizer(params_.learning_rate, params_.beta1, params_.beta2);
    network_ =
        new Network(params_.input_size, params_.hidden_size,
                    params_.output_size, optimizer_, params_.learning_rate);
    if (withMonitoring) {
      auto monitor = buildMonitor(network_);
      network_->SetMonitor(monitor);
    }
    return true;

  } catch (std::exception &ex) {
    std::cerr << "[ERROR] " << ex.what() << std::endl;
    return false;
  }
}

bool SimpleMLP::train() {
  std::cout << "Training, using file " << params_.data_file << std::endl;
  Training training(network_, params_.data_file, optimizer_);
  return training.Train(params_.num_epochs, params_.output_at_end, 0,
                        params_.to_line);
}

bool SimpleMLP::test() {
  std::cout << "Testing, using file " << params_.data_file << std::endl;
  Testing testing(network_, params_.data_file);
  return testing.Test(params_.output_at_end, params_.to_line, 0);
}

int SimpleMLP::parseArgs(int argc, char **argv) {

  CLI::App app{params_.title};

  app.add_option("-f,--file_input", params_.data_file,
                 "the data file to use for training and testing")
      ->mandatory()
      ->check(CLI::ExistingPath);
  app.add_option("-i,--input_size", params_.input_size,
                 "the numbers of input neurons")
      ->mandatory()
      ->check(CLI::PositiveNumber);
  app.add_option("-s,--hidden_size", params_.hidden_size,
                 "the numbers of hidden neurons")
      ->default_val(params_.hidden_size)
      ->check(CLI::PositiveNumber);
  app.add_option("-o,--output_size", params_.output_size,
                 "the numbers of output neurons")
      ->default_val(params_.output_size)
      ->check(CLI::PositiveNumber);
  app.add_option("-e,--epochs", params_.num_epochs,
                 "the numbers of epochs retraining")
      ->default_val(params_.num_epochs)
      ->check(CLI::PositiveNumber);
  app.add_option(
         "-l,--line_to", params_.to_line,
         "the line number until the training will complete and testing will "
         "start, or 0 to use the entire file")
      ->default_val(params_.to_line)
      ->check(CLI::NonNegativeNumber);
  app.add_option("-w,--learning_rate", params_.learning_rate,
                 "optimizer learning rate")
      ->default_val(params_.learning_rate)
      ->check(CLI::TypeValidator<float>());
  app.add_option("-x,--beta1", params_.beta1, "optimizer beta1")
      ->default_val(params_.beta1)
      ->check(CLI::TypeValidator<float>());
  app.add_option("-y,--beta2", params_.beta2, "optimizer beta2")
      ->default_val(params_.beta2)
      ->check(CLI::TypeValidator<float>());
  app.add_option(
         "-z,--output_ends", params_.output_at_end,
         "indicate if the output data is at the end of the record (1) or at "
         "the beginning (0)")
      ->default_val(params_.output_at_end)
      ->check(CLI::TypeValidator<bool>());

  CLI11_PARSE(app, argc, argv)
  return EXIT_SUCCESS;
}

Monitor *SimpleMLP::buildMonitor(Network *network) {
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
  for (auto connection : network->GetInputLayer()->connections) {
    ss << "ic" << connection->from->id << "-" << connection->to->id;
    monitor->log(ss.str());
    ss.str("");
    ss.clear();
  }
  size_t hs = 0;
  for (auto const &hidden : network->GetHiddenLayers()) {
    hs++;
    for (auto const &con : hidden->connections) {
      ss << "h" << hs << "c" << con->from->id << "-" << con->to->id;
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