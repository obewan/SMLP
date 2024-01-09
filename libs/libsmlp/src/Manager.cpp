#include "Manager.h"
#include "exception/ManagerException.h"

void Manager::predict(const std::string &line) {
  // no log here as the output is the result
  if (!predict_) {
    predict_ = std::make_unique<Predict>();
  }
  predict_->predict(line);
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

  if (app_params.use_socket && app_params.verbose) {
    logger.info("Training using sockets, received line: ", line);
  } else if (app_params.use_stdin) {
    handleStdinTraining();
  } else {
    handleFileTraining();
  }

  training_->train(line);
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

  if (app_params.use_socket && app_params.verbose) {
    logger.info("Testing using sockets, received line: ", line);
  } else if (app_params.use_stdin) {
    handleStdinTesting();
  } else {
    handleFileTesting();
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

  if (app_params.use_socket && app_params.verbose) {
    logger.info("Train and testing using sockets, received line: ", line);
  }

  if (app_params.use_stdin) {
    logger.info("Train and testing, using command pipe input...");
    logger.info("OutputIndexToMonitor:", app_params.output_index_to_monitor,
                " ", showInlineHeader());
  }

  if (!app_params.use_stdin && !app_params.use_socket) {
    logger.info("Train and testing, using file ", app_params.data_file);
    logger.info("OutputIndexToMonitor:", app_params.output_index_to_monitor,
                " ", showInlineHeader());
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

void Manager::processTCPClient(const std::string &line) {
  if (!app_params.use_socket) {
    throw ManagerException("TCP socket not set.");
  }
  switch (app_params.mode) {
  case EMode::Predictive:
    predict(line);
    break;
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
    throw ManagerException("Unimplemented mode.");
  }
}