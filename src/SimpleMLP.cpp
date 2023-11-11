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
#include "SimpleLang.h"
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

int SimpleMLP::init(int argc, char **argv) {
  try {
    if (argc == 1) {
      argv[1] = (char *)"-h"; // showing help by default
      argc++;
    }
    if (int init = parseArgs(argc, argv); init != EXIT_SUCCESS) {
      return init;
    }

    // Some post validations
    if (app_params.data_file.empty()) {
      logger.error("A dataset file is required, but file_input is missing.");
      return EXIT_FAILURE;
    }

    if (app_params.mode == EMode::Predictive &&
        app_params.network_to_import.empty()) {
      logger.error("Predictive mode require a trained network to import, but "
                   "network_to_import is missing.");
      return EXIT_FAILURE;
    }

    importOrBuildNetwork();

    return EXIT_SUCCESS;

  } catch (std::exception &ex) {
    logger.error(ex.what());
    return EXIT_FAILURE;
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

int SimpleMLP::parseArgs(int argc, char **argv) {
  SimpleConfig config(app_params.config_file);
  SimpleLang lang(config.lang_file);
  CLI::App app{app_params.title};
  bool version = false;

  // valid a parent path, if there is a path, that include a futur filename (not
  // like CLI::ExistingPath, CLI::ExistingDirectory or CLI::ExistingFile)
  auto valid_path = [](auto filename) {
    if (std::filesystem::path p(filename);
        p.has_parent_path() && !std::filesystem::exists(p.parent_path())) {
      return std::string("The directory of the file does not exist.");
    }
    return std::string();
  };

  auto addOption = [&app, &lang](const auto &name, auto &param,
                                 auto... validators) {
    auto option = app.add_option(name, param, lang.get(name));
    option->default_val(param);
    (option->check(validators), ...);
    return option;
  };

  auto addOptionTransform = [&app, &lang](const auto &name, auto &param,
                                          auto transform) {
    auto option = app.add_option(name, param, lang.get(name));
    option->default_val(param);
    option->transform(transform);
    return option;
  };

  auto addFlag = [&app, &lang](const auto &name, auto &param) {
    return app.add_flag(name, param, lang.get(name));
  };

  addOption("-i,--import_network", app_params.network_to_import,
            CLI::ExistingFile);
  addOption("-e,--export_network", app_params.network_to_export, valid_path);
  addOption("-f,--file_input", app_params.data_file, CLI::ExistingFile);
  addOption("-s,--input_size", network_params.input_size, CLI::PositiveNumber);
  addOption("-o,--output_size", network_params.output_size,
            CLI::PositiveNumber);
  addOption("-d,--hidden_size", network_params.hidden_size,
            CLI::NonNegativeNumber);
  addOption("-c,--hiddens_count", network_params.hiddens_count,
            CLI::NonNegativeNumber);
  addOption("-p,--epochs", app_params.num_epochs, CLI::NonNegativeNumber);
  addOption("-l,--learning_rate", network_params.learning_rate,
            CLI::Range(0.0f, 1.0f), CLI::TypeValidator<float>());
  addFlag("-t,--output_ends", app_params.output_at_end);
  addOption("-r,--training_ratio", app_params.training_ratio,
            CLI::Range(0.0f, 1.0f), CLI::TypeValidator<float>());
  addOptionTransform("-m,--mode", app_params.mode,
                     CLI::CheckedTransformer(mode_map, CLI::ignore_case));
  addOptionTransform("-n,--predictive_mode", app_params.predictive_mode,
                     CLI::CheckedTransformer(predictive_map, CLI::ignore_case));
  addOption("-y,--output_index_to_monitor", app_params.output_index_to_monitor,
            CLI ::NonNegativeNumber);
  addOptionTransform("-a,--hidden_activation_function",
                     network_params.hidden_activation_function,
                     CLI::CheckedTransformer(activation_map, CLI::ignore_case));
  addOptionTransform("-b,--output_activation_function",
                     network_params.output_activation_function,
                     CLI::CheckedTransformer(activation_map, CLI::ignore_case));
  addOption("-k,--hidden_activation_alpha",
            network_params.hidden_activation_alpha,
            CLI::Range(-100.0f, 100.0f));
  addOption("-q,--output_activation_alpha",
            network_params.output_activation_alpha,
            CLI::Range(-100.0f, 100.0f));
  addFlag("-v,--version", version);
  addFlag("-w,--verbose", app_params.verbose);

  // Parsing
  try {
    app.parse(argc, argv);
  } catch (const CLI::CallForHelp &e) {
    // This is returned when -h or --help is called
    app.exit(e);
    return EXIT_HELP;
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  // Version special exit
  if (version) {
    logger.out(app_params.title, " v", app_params.version);
    logger.out("Copyright Damien Balima (https://dams-labs.net) 2023");
    return EXIT_VERSION;
  }

  // Config file settings
  ConfigSettings(config);

  return EXIT_SUCCESS;
}

void SimpleMLP::ConfigSettings(const SimpleConfig &config) {
  if (config.isValidConfig) {
    logger.info("Using config file ", config.config_file, "...");
  } else {
    logger.info("No valid config file ", config.config_file, " found...");
  }
  if (!config.file_input.empty() && app_params.data_file.empty()) {
    app_params.data_file = config.file_input;
  }
  if (!config.import_network.empty() && app_params.network_to_import.empty()) {
    app_params.network_to_import = config.import_network;
  }
  if (!config.export_network.empty() && app_params.network_to_export.empty()) {
    app_params.network_to_export = config.export_network;
  }
}

void SimpleMLP::runMode() {
  switch (app_params.mode) {
  case EMode::Predictive:
    predict();
    break;
  case EMode::TrainOnly:
    train();
    break;
  case EMode::TestOnly:
    test();
    break;
  case EMode::TrainTestMonitored:
    trainTestMonitored();
    break;
  case EMode::TrainThenTest:
    train();
    test();
    break;
  default:
    throw std::runtime_error("Unimplemented mode.");
  }
}

void SimpleMLP::importOrBuildNetwork() {
  if (!app_params.network_to_import.empty() &&
      (app_params.mode == EMode::Predictive ||
       app_params.mode == EMode::TestOnly)) {
    if (app_params.mode !=
        EMode::Predictive) { // avoiding header lines on
                             // this mode, for commands chaining with pipes
      logger.info("Importing network model from ", app_params.network_to_import,
                  "...");
    }
    network =
        std::unique_ptr<Network>(importExportJSON.importModel(app_params));
    network_params = network->params;
  } else {
    network = std::make_unique<Network>(network_params);
  }
}

bool SimpleMLP::shouldExportNetwork() {
  return !app_params.network_to_export.empty() &&
         (app_params.mode == EMode::TrainOnly ||
          app_params.mode == EMode::TrainTestMonitored ||
          app_params.mode == EMode::TrainThenTest);
}

void SimpleMLP::exportNetwork() {
  logger.info("Exporting network model to ", app_params.network_to_export,
              "...");
  importExportJSON.exportModel(network.get(), app_params);
}