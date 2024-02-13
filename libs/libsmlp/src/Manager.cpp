#include "Manager.h"
#include "CommonModes.h"
#include "CommonResult.h"
#include "SimpleLang.h"
#include "exception/ManagerException.h"
#include <exception>

Common::Result Manager::predict(const std::string &line) {
  // no log here as the output is the result
  if (!predict_) {
    predict_ = std::make_unique<Predict>();
  }
  return predict_->predict(line);
}

void Manager::train(const std::string &line) {
  auto handleStdinTraining = [this]() {
    logger.info("Training, using command pipe input...");
    if (app_params.training_ratio_line == 0 || app_params.num_epochs > 1) {
      logger.warn("Epochs and training ratio are disabled using command "
                  "pipe input. Use training_ratio_line parameter instead.");
    }
    logger.info(showInlineHeader());
  };

  auto handleFileTraining = [this]() {
    logger.info("Training, using file ", app_params.data_file);
    logger.info(showInlineHeader());
  };

  if (!training_) {
    createTraining();
  }

  switch (app_params.input) {
  case EInput::File:
    handleFileTraining();
    break;
  case EInput::Stdin:
    handleStdinTraining();
    break;
  case EInput::Socket:
    if (app_params.verbose) {
      logger.info("Training using TCP socket...");
    }
    break;
  default:
    break;
  }

  try {
    const auto &results = training_->train(line);
    if (app_params.verbose) {
      if (results.isSuccess()) {
        logger.info("Training success.");
      } else {
        logger.error("Training error.");
      }
    }
  } catch (std::exception &ex) {
    logger.error("Training error: ", ex.what());
  }
}

void Manager::test(const std::string &line) {
  auto handleStdinTesting = [this]() {
    logger.info("Testing, using command pipe input... ", app_params.data_file);
  };

  auto handleFileTesting = [this]() {
    logger.info("Testing, using file ", app_params.data_file);
  };

  if (!testing_) {
    createTesting();
  }

  switch (app_params.input) {
  case EInput::File:
    handleFileTesting();
    break;
  case EInput::Stdin:
    handleStdinTesting();
    break;
  case EInput::Socket:
    if (app_params.verbose) {
      logger.info("Testing using TCP socket...");
    }
    break;
  default:
    break;
  }

  testing_->test(line);
  logger.out(testing_->getTestingResults()->showDetailledResults());
}

void Manager::trainTestMonitored(const std::string &line) {
  if (app_params.output_index_to_monitor > network_params.output_size) {
    logger.error("output_index_to_monitor > output_size: ",
                 app_params.output_index_to_monitor, ">",
                 network_params.output_size);
    return;
  }

  switch (app_params.input) {
  case EInput::File:
    logger.info("Train and testing, using file ", app_params.data_file);
    logger.info("OutputIndexToMonitor:", app_params.output_index_to_monitor,
                " ", showInlineHeader());
    break;
  case EInput::Stdin:
    logger.info("Train and testing, using command pipe input...");
    logger.info("OutputIndexToMonitor:", app_params.output_index_to_monitor,
                " ", showInlineHeader());
    break;
  case EInput::Socket:
    if (app_params.verbose) {
      logger.info("Train and testing using TCP socket...");
    }
    break;
  default:
    break;
  }

  if (!training_) {
    createTraining();
  }
  training_->train(line);
}

std::string Manager::showInlineHeader() const {
  std::stringstream sst;
  sst << "InputSize:" << network_params.input_size
      << " OutputSize:" << network_params.output_size
      << " HiddenSize:" << network_params.hidden_size
      << " HiddenLayers:" << network_params.hiddens_count
      << " LearningRate:" << network_params.learning_rate
      << " HiddenActivationFunction:"
      << Common::GetActivationStr(network_params.hidden_activation_function);
  if (network_params.hidden_activation_function == EActivationFunction::ELU ||
      network_params.hidden_activation_function == EActivationFunction::PReLU) {
    sst << " HiddenActivationAlpha:" << network_params.hidden_activation_alpha;
  }
  sst << " OutputActivationFunction:"
      << Common::GetActivationStr(network_params.output_activation_function);
  if (network_params.output_activation_function == EActivationFunction::ELU ||
      network_params.output_activation_function == EActivationFunction::PReLU) {
    sst << " OutputActivationAlpha:" << network_params.output_activation_alpha;
  }
  if (app_params.input != EInput::Stdin) {
    sst << " Epochs:" << app_params.num_epochs;
    if (app_params.training_ratio_line > 0) {
      sst << " TrainingRatioLine:" << app_params.training_ratio_line;
    } else {
      sst << " TrainingRatio:" << app_params.training_ratio;
    }
  } else {
    sst << " TrainingRatioLine:" << app_params.training_ratio_line;
  }
  sst << " Mode:" << Common::GetModeStr(app_params.mode)
      << " Verbose:" << app_params.verbose;
  return sst.str();
}

void Manager::runMode() {
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
    throw ManagerException("Unimplemented mode.");
  }
}

void Manager::importOrBuildNetwork() {
  // avoiding header lines on Predictive mode, for commands chaining with
  // pipes
  auto logNetworkImport = [this]() {
    if (app_params.mode != EMode::Predictive) {
      logger.info("Importing network model from ", app_params.network_to_import,
                  "...");
    }
  };
  auto logNetworkCreation = [this]() {
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

bool Manager::shouldExportNetwork() const {
  return !app_params.network_to_export.empty() &&
         app_params.mode != EMode::Predictive &&
         app_params.mode != EMode::TestOnly;
}

void Manager::exportNetwork() {
  logger.info("Exporting network model to ", app_params.network_to_export,
              "...");
  importExportJSON.exportModel(network.get(), app_params);
}

Common::Result Manager::processTCPClient(const std::string &line) {
  if (app_params.input != EInput::Socket) {
    throw ManagerException(SimpleLang::Error(Error::TCPSocketNotSet));
  }
  switch (app_params.mode) {
  case EMode::Predictive:
    return predict(line);
  case EMode::TrainOnly:
    train(line);
    break;
  case EMode::TestOnly:
    test(line);
    break;
  case EMode::TrainTestMonitored:
    trainTestMonitored(line);
    break;
  case EMode::TrainThenTest:
    train(line);
    test(line);
    break;
  default:
    throw ManagerException(SimpleLang::Error(Error::UnimplementedMode));
  }
  return {.code = Common::make_error_code(Common::ErrorCode::Success)};
}