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
#include "Common.h"
#include "Network.h"
#include "Predict.h"
#include "Testing.h"
#include "Training.h"
#include "include/CLI11.hpp"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

bool SimpleMLP::init(int argc, char **argv, bool &showVersion) {
  try {
    if (argc == 1) {
      argv[1] = (char *)"-h"; // showing help by default
      argc++;
    }
    if (parseArgs(argc, argv, showVersion) != EXIT_SUCCESS ||
        app_params.data_file.empty()) { // in case of show help or show version
      return false;
    }

    // Some post validations
    if (app_params.mode == EMode::Predictive &&
        app_params.network_to_import.empty()) {
      logger.error("Predictive mode require a trained network to import, but "
                   "network_to_import is missing.");
      return false;
    }

    // Instantiation of the network
    if (!app_params.network_to_import.empty()) {
      if (app_params.mode !=
          EMode::Predictive) { // avoiding header lines on
                               // this mode, for commands chaining
        logger.info("Importing network model from ",
                    app_params.network_to_import, "...");
      }
      network =
          std::unique_ptr<Network>(importExportJSON.importModel(app_params));
      network_params = network->params;
    } else {
      network = std::make_unique<Network>(network_params);
    }
    return true;

  } catch (std::exception &ex) {
    logger.error(ex.what());
    return false;
  }
}

void SimpleMLP::predict() {
  auto predic = std::make_unique<Predict>(network.get(), app_params, logger);
  predic->predict();
}

void SimpleMLP::train() {
  logger.info("Training, using file ", app_params.data_file);
  logger.info(showInlineHeader());
  Training training(network.get(), app_params.data_file, logger);
  training.train(network_params, app_params);
}

void SimpleMLP::test() {
  logger.info("Testing, using file ", app_params.data_file);
  Testing testing(network.get(), app_params.data_file);
  testing.test(network_params, app_params, 0);
  logger.out(testing.showResults(app_params.mode));
}

void SimpleMLP::trainTestMonitored() {
  if (app_params.output_index_to_monitor > network_params.output_size) {
    logger.error("output_index_to_monitor > output_size: ",
                 app_params.output_index_to_monitor, ">",
                 network_params.output_size);
    return;
  }

  logger.info("Train and testing, using file ", app_params.data_file);
  logger.info("OutputIndexToMonitor:", app_params.output_index_to_monitor,
              showInlineHeader());
  Training training(network.get(), app_params.data_file, logger);
  training.trainTestMonitored(network_params, app_params);
}

std::string SimpleMLP::showInlineHeader() const {
  std::stringstream sst;
  sst << " InputSize:" << network_params.input_size
      << " OutputSize:" << network_params.output_size
      << " HiddenSize:" << network_params.hidden_size
      << " HiddenLayers:" << network_params.hiddens_count
      << " LearningRate:" << network_params.learning_rate
      << " HiddenActivationFunction:"
      << Common::getActivationStr(network_params.hidden_activation_function);
  if (network_params.hidden_activation_function == EActivationFunction::ELU ||
      network_params.hidden_activation_function == EActivationFunction::PReLU) {
    sst << " HiddenActivationAlpha:" << network_params.hidden_activation_alpha;
  }
  sst << " OutputActivationFunction:"
      << Common::getActivationStr(network_params.output_activation_function);
  if (network_params.output_activation_function == EActivationFunction::ELU ||
      network_params.output_activation_function == EActivationFunction::PReLU) {
    sst << " OutputActivationAlpha:" << network_params.output_activation_alpha;
  }
  sst << " Epochs:" << app_params.num_epochs
      << " TrainingRatio:" << app_params.training_ratio
      << " Mode:" << Common::getModeStr(app_params.mode)
      << " Verbose:" << app_params.verbose;
  return sst.str();
}

int SimpleMLP::parseArgs(int argc, char **argv, bool &showVersion) {

  CLI::App app{app_params.title};

  // valid a parent path, if there is a path, that include a futur filename (not
  // like CLI::ExistingPath, CLI::ExistingDirectory or CLI::ExistingFile)
  auto valid_path = [](auto filename) {
    if (std::filesystem::path p(filename);
        p.has_parent_path() && !std::filesystem::exists(p.parent_path())) {
      return std::string("The directory of the file does not exist.");
    }
    return std::string();
  };

  auto addOption = [&app](auto &name, auto &param, auto &description,
                          auto... validators) {
    auto option = app.add_option(name, param, description);
    option->default_val(param);
    (option->check(validators), ...);
  };

  auto addOptionTransform = [&app](auto &name, auto &param, auto &description,
                                   auto transform) {
    auto option = app.add_option(name, param, description);
    option->default_val(param);
    option->transform(transform);
  };

  auto addFlag = [&app](auto &name, auto &param, auto &description) {
    app.add_flag(name, param, description);
  };

  addOption("-a,--import_network", app_params.network_to_import,
            "Import a network model instead of creating a new one. This must "
            "be a valid model filepath, specifically a file generated by SMLP. "
            "If this option is used, there is no need to specify layer "
            "parameters as they are included in the model.",
            CLI::ExistingFile);
  addOption("-b, --export_network", app_params.network_to_export,
            "Export the network model after training. This must be a valid "
            "filepath. The exported model can be imported later, eliminating "
            "the need for retraining.",
            valid_path);
  addOption("-f,--file_input", app_params.data_file,
            "Specify the data file to be used for training and testing.",
            CLI::ExistingPath);
  addOption("-i,--input_size", network_params.input_size,
            "The numbers of input neurons", CLI::PositiveNumber);
  addOption("-o,--output_size", network_params.output_size,
            "The numbers of output neurons", CLI::PositiveNumber);
  addOption("-d,--hidden_size", network_params.hidden_size,
            "The numbers of hidden neurons per hidden layer",
            CLI::NonNegativeNumber);
  addOption("-c,--hiddens_count", network_params.hiddens_count,
            "The count of hidden layers", CLI::NonNegativeNumber);
  addOption("-e,--epochs", app_params.num_epochs,
            "The numbers of epochs retraining", CLI::NonNegativeNumber);
  addOption("-r,--learning_rate", network_params.learning_rate,
            "The network training learning rate", CLI::Range(0.0f, 1.0f),
            CLI::TypeValidator<float>());
  addFlag("-s,--output_ends", app_params.output_at_end,
          "Indicate that the output columns of the dataset are located at the "
          "end of the record, "
          "by default smlp will look at the firsts columns");
  addOption("-t,--training_ratio", app_params.training_ratio,
            "The training ratio of the file to switch between data for "
            "training and data for testing, should be around 0.7.",
            CLI::Range(0.0f, 1.0f), CLI::TypeValidator<float>());
  addOptionTransform(
      "-m, --mode", app_params.mode,
      "Select the running mode:\n"
      "  - Predictive:This mode uses an input file to predict the outputs. "
      "If the input file contains output columns, the predicted CSV outputs "
      "will replace them without modifying the original input file. Please "
      "be mindful of the parameters (input_size, output_size, output_ends). "
      "If the input file does not contain output columns, pay close "
      "attention to the input_size parameter. This mode requires a network "
      "that has been imported and trained (be sure that the model has good "
      "testing results).\n"
      "  - TestOnly: Test an imported network without training.\n"
      "  - TrainOnly: Train the network without testing.\n"
      "  - TrainThenTest: Train and then test the network (default).\n"
      "  - TrainTestMonitored: Train and test at each epoch while "
      "monitoring the progress of an output neuron. Be aware that this is "
      "slower and uses more memory.",
      CLI::CheckedTransformer(mode_map, CLI::ignore_case));
  addOptionTransform(
      "-n, --predictive_mode", app_params.predictive_mode,
      "If using Predictive mode, select the output render mode:\n"
      "  - CSV: This will render the output(s) at the end or at the "
      "begining of the input line, "
      "depending of your output_ends option (default).\n"
      "  - NumberAndRaw: This will show both the predicted output(s) "
      "numbers and their raw values.\n"
      "  - NumberOnly: This will show only the predicted outputs number.\n"
      "  - RawOnly: This will only show the output(s) raw values.",
      CLI::CheckedTransformer(predictive_map, CLI::ignore_case));
  addOption("-y, --output_index_to_monitor", app_params.output_index_to_monitor,
            "Indicate the output neuron index to monitor during a "
            "TrainTestMonitored mode. If index = 0 there will be no "
            "progress monitoring. Default is 1, the first neuron output.",
            CLI ::NonNegativeNumber);
  addOptionTransform("-j, --hidden_activation_function",
                     network_params.hidden_activation_function,
                     "Select the hidden neurons activation function:\n"
                     "  - ELU: Exponential Linear Units, require an "
                     "hidden_activation_alpha parameter.\n"
                     "  - LReLU: Leaky ReLU.\n"
                     "  - PReLU: Parametric ReLU, require an "
                     "hidden_activation_alpha_parameter.\n"
                     "  - ReLU: Rectified Linear Unit.\n"
                     "  - Sigmoid (default).\n"
                     "  - Tanh: Hyperbolic Tangent",
                     CLI::CheckedTransformer(activation_map, CLI::ignore_case));
  addOptionTransform("-k, --output_activation_function",
                     network_params.output_activation_function,
                     "Select the output neurons activation function:\n"
                     "  - ELU: Exponential Linear Units, require an "
                     "output_activation_alpha parameter.\n"
                     "  - LReLU: Leaky ReLU.\n"
                     "  - PReLU: Parametric ReLU, require an "
                     "output_activation_alpha parameter.\n"
                     "  - ReLU: Rectified Linear Unit.\n"
                     "  - Sigmoid (default).\n"
                     "  - Tanh: Hyperbolic Tangent",
                     CLI::CheckedTransformer(activation_map, CLI::ignore_case));
  addOption("-p, --hidden_activation_alpha",
            network_params.hidden_activation_alpha,
            "alpha parameter value for ELU and PReLU activation functions "
            "on hidden layers",
            CLI::Range(-100.0f, 100.0f));
  addOption("-q, --output_activation_alpha",
            network_params.output_activation_alpha,
            "alpha parameter value for ELU and PReLU activation functions "
            "on output layer",
            CLI::Range(-100.0f, 100.0f));
  addFlag("-v,--version", showVersion, "Show current version");
  addFlag("-w,--verbose", app_params.verbose, "Verbose logs");

  CLI11_PARSE(app, argc, argv) return EXIT_SUCCESS;
}
