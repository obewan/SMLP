/**
 * \mainpage SMLP
 * A Simple Multi-Layer Perceptron to study artificial networks.
 * \author  Damien Balima (https://dams-labs.net)
 * \date    August 2023
 * \see https://github.com/obewan/SMLP
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
  const auto &logger = SimpleLogger::getInstance();
  try {
    checkStdin(); // check in case using defaults options

    if (argc == 1 && app_params.input != EInput::Stdin) {
      argv[1] = (char *)"-h"; // showing help by default
      argc++;
    }

    SimpleConfig config(app_params.config_file);
    SimpleLang::getInstance().parseFile(config.lang_file);

    if (int init = parseArgs(argc, argv); init != EXIT_SUCCESS) {
      return init;
    }

    // check again but including -x param (stdin disable) this time
    checkStdin();
    // Config file settings, must be after this second checkStdin()
    ConfigSettings(config);

    // Some post validations
    if (app_params.data_file.empty() && app_params.input == EInput::File) {
      logger.error(SimpleLang::Error(Error::InvalidDatasetFileMissing));
      return EXIT_FAILURE;
    }
    if (app_params.mode == EMode::Predict &&
        app_params.network_to_import.empty()) {
      logger.error(SimpleLang::Error(Error::InvalidPredictNetworkMissing));
      return EXIT_FAILURE;
    }
    if (app_params.disable_stdin && app_params.mode != EMode::Predict) {
      logger.info(SimpleLang::Message(Message::StdinDisabled));
    }

    Manager::getInstance().importOrBuildNetwork();

    return EXIT_SUCCESS;

  } catch (std::exception &ex) {
    logger.error(ex.what());
    return EXIT_FAILURE;
  }
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
      return SimpleLang::Error(Error::InvalidDirectory);
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
  addOptionTransform("-n,--predict_mode", app_params.predict_mode,
                     CLI::CheckedTransformer(predict_map, CLI::ignore_case));
  addOption("-y,--output_index_to_monitor", app_params.output_index_to_monitor,
            CLI ::NonNegativeNumber);
  addOptionTransform("-a,--hidden_activation_function",
                     network_params.hidden_activation_function,
                     CLI::CheckedTransformer(activation_map, CLI::ignore_case));
  addOptionTransform("-b,--output_activation_function",
                     network_params.output_activation_function,
                     CLI::CheckedTransformer(activation_map, CLI::ignore_case));
  addOption("-A,--hidden_activation_alpha",
            network_params.hidden_activation_alpha,
            CLI::Range(-100.0f, 100.0f));
  addOption("-B,--output_activation_alpha",
            network_params.output_activation_alpha,
            CLI::Range(-100.0f, 100.0f));
  addFlag("-x,--disable_stdin", app_params.disable_stdin);
  addFlag("-v,--version", version);
  addFlag("-V,--verbose", app_params.verbose);

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
    const auto &logger = SimpleLogger::getInstance();
    logger.out(app_params.title, " v", app_params.version);
    logger.out("Copyright Damien Balima (https://dams-labs.net) 2023");
    return EXIT_VERSION;
  }

  return EXIT_SUCCESS;
}

void SimpleMLP::ConfigSettings(const SimpleConfig &config) {
  const auto &logger = SimpleLogger::getInstance();
  if (app_params.mode != EMode::Predict) {
    if (config.isValidConfig) {
      logger.info(SimpleLang::Message(Message::UsingConfigFile,
                                      {{"config_file", config.config_file}}));
    } else if (app_params.verbose) {
      logger.info(SimpleLang::Error(Error::ConfigFileNotFound,
                                    {{"config_file", config.config_file}}));
    }
  }
  if (!config.file_input.empty() && app_params.data_file.empty() &&
      app_params.input == EInput::File) {
    app_params.data_file = config.file_input;
  }
  if (!config.import_network.empty() && app_params.network_to_import.empty()) {
    app_params.network_to_import = config.import_network;
  }
  if (!config.export_network.empty() && app_params.network_to_export.empty()) {
    app_params.network_to_export = config.export_network;
  }
}

void SimpleMLP::checkStdin() {
  if (app_params.input == EInput::Socket) {
    return;
  }
  app_params.input = EInput::File; // important
  if (!app_params.disable_stdin && !ISATTY(FILENO(stdin))) {
    app_params.input = EInput::Stdin;
  }
}