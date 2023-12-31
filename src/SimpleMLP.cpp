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
#include "SimpleConfig.h"
#include "SimpleLang.h"
#include "Testing.h"
#include "TestingResult.h"
#include "Training.h"
#include "exception/SmlpException.h"
#include "include/CLI11.hpp"
#include <SimpleLogger.h>
#include <filesystem>
#include <memory>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

int SimpleMLP::init(int argc, char **argv) {
  const auto &logger = SimpleLogger::getIntance();
  try {
    checkStdin(); // check in case using defaults options

    if (argc == 1 && !app_params.use_stdin) {
      argv[1] = (char *)"-h"; // showing help by default
      argc++;
    }

    SimpleConfig config(app_params.config_file);
    SimpleLang::getInstance().parseFile(config.lang_file);

    if (int init = parseArgs(argc, argv); init != EXIT_SUCCESS) {
      return init;
    }

    // check again but including -x param this time
    checkStdin();
    // Config file settings, must be after this second checkStdin()
    ConfigSettings(config);

    // Some post validations
    if (app_params.data_file.empty() && !app_params.use_stdin) {
      logger.error("A dataset file is required, but file_input is missing.");
      return EXIT_FAILURE;
    }
    if (app_params.mode == EMode::Predictive &&
        app_params.network_to_import.empty()) {
      logger.error("Predictive mode require a trained network to import, but "
                   "network_to_import is missing.");
      return EXIT_FAILURE;
    }
    if (app_params.disable_stdin && app_params.mode != EMode::Predictive) {
      logger.info("Stdin disabled");
    }

    importOrBuildNetwork();

    return EXIT_SUCCESS;

  } catch (std::exception &ex) {
    logger.error(ex.what());
    return EXIT_FAILURE;
  }
}

void SimpleMLP::predict() {
  auto predict = std::make_unique<Predict>(network, app_params);
  predict->predict();
}

void SimpleMLP::train() {
  const auto &logger = SimpleLogger::getIntance();
  if (app_params.use_stdin) {
    logger.info("Training, using command pipe input...");
    if (app_params.training_ratio_line == 0 || app_params.num_epochs > 1) {
      logger.warn("Epochs and training ratio are disabled using command "
                  "pipe input. Use training_ratio_line parameter instead.");
    }
  } else {
    logger.info("Training, using file ", app_params.data_file);
  }
  logger.info(showInlineHeader());
  auto training = std::make_unique<Training>(network, app_params.data_file);
  training->train(network_params, app_params);
}

void SimpleMLP::test() {
  const auto &logger = SimpleLogger::getIntance();
  auto testing = std::make_unique<Testing>(network, app_params.data_file);
  if (app_params.use_stdin) {
    logger.info("Testing, using command pipe input... ", app_params.data_file);
  } else {
    logger.info("Testing, using file ", app_params.data_file);
  }
  testing->test(network_params, app_params);
  logger.out(
      testing->getTestingResults()->showDetailledResults(app_params.mode));
}

void SimpleMLP::trainTestMonitored() {
  const auto &logger = SimpleLogger::getIntance();
  if (app_params.output_index_to_monitor > network_params.output_size) {
    logger.error("output_index_to_monitor > output_size: ",
                 app_params.output_index_to_monitor, ">",
                 network_params.output_size);
    return;
  }

  if (app_params.use_stdin) {
    logger.info("Train and testing, using command pipe input...");
  } else {
    logger.info("Train and testing, using file ", app_params.data_file);
  }
  logger.info("OutputIndexToMonitor:", app_params.output_index_to_monitor, " ",
              showInlineHeader());
  Training training(network, app_params.data_file);
  training.train(network_params, app_params);
}

std::string SimpleMLP::showInlineHeader() const {
  std::stringstream sst;
  sst << "InputSize:" << network_params.input_size
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
  if (!app_params.use_stdin) {
    sst << " Epochs:" << app_params.num_epochs;
    if (app_params.training_ratio_line > 0) {
      sst << " TrainingRatioLine:" << app_params.training_ratio_line;
    } else {
      sst << " TrainingRatio:" << app_params.training_ratio;
    }
  } else {
    sst << " TrainingRatioLine:" << app_params.training_ratio_line;
  }
  sst << " Mode:" << Common::getModeStr(app_params.mode)
      << " Verbose:" << app_params.verbose;
  return sst.str();
}

int SimpleMLP::parseArgs(int argc, char **argv) {
  CLI::App app{app_params.title};
  bool version = false;
  const auto &lang = SimpleLang::getInstance();

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
  addOption("-R,--training_ratio_line", app_params.training_ratio_line,
            CLI::NonNegativeNumber);
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
  addFlag("-x,--disable_stdin", app_params.disable_stdin);
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
    const auto &logger = SimpleLogger::getIntance();
    logger.out(app_params.title, " v", app_params.version);
    logger.out("Copyright Damien Balima (https://dams-labs.net) 2023");
    return EXIT_VERSION;
  }

  return EXIT_SUCCESS;
}

void SimpleMLP::ConfigSettings(const SimpleConfig &config) {
  const auto &logger = SimpleLogger::getIntance();
  if (app_params.mode != EMode::Predictive) {
    if (config.isValidConfig) {
      logger.info("Using config file ", config.config_file, "...");
    } else {
      logger.info("No valid config file ", config.config_file, " found...");
    }
  }
  if (!config.file_input.empty() && app_params.data_file.empty() &&
      !app_params.use_stdin) {
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
    throw SmlpException("Unimplemented mode.");
  }
}

void SimpleMLP::importOrBuildNetwork() {
  const auto &logger = SimpleLogger::getIntance();
  // avoiding header lines on Predictive mode, for commands chaining with pipes
  auto logNetworkImport = [this, &logger]() {
    if (app_params.mode != EMode::Predictive) {
      logger.info("Importing network model from ", app_params.network_to_import,
                  "...");
    }
  };
  auto logNetworkCreation = [this, &logger]() {
    if (!app_params.network_to_import.empty() &&
        app_params.mode != EMode::Predictive) {
      logger.info("Network model ", app_params.network_to_import,
                  " not found, creating a new one...");
    }
  };

  if (!app_params.network_to_import.empty() &&
      std::filesystem::exists(app_params.network_to_import)) {
    logNetworkImport();
    network =
        std::shared_ptr<Network>(importExportJSON.importModel(app_params));
    network_params = network->params;
  } else {
    logNetworkCreation();
    network = std::make_shared<Network>(network_params);
  }
}

bool SimpleMLP::shouldExportNetwork() const {
  return !app_params.network_to_export.empty() &&
         (app_params.mode == EMode::TrainOnly ||
          app_params.mode == EMode::TrainTestMonitored ||
          app_params.mode == EMode::TrainThenTest);
}

void SimpleMLP::exportNetwork() {
  const auto &logger = SimpleLogger::getIntance();
  logger.info("Exporting network model to ", app_params.network_to_export,
              "...");
  importExportJSON.exportModel(network.get(), app_params);
}

void SimpleMLP::checkStdin() {
  app_params.use_stdin = false; // important
  if (!app_params.disable_stdin) {
    app_params.use_stdin = !ISATTY(FILENO(stdin));
  }
}