#include "Manager.h"
#include "Common.h"
#include "CommonModes.h"
#include "CommonParameters.h"
#include "CommonResult.h"
#include "RunnerFileVisitor.h"
#include "RunnerSocketVisitor.h"
#include "RunnerStdinVisitor.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "exception/ManagerException.h"
#include <exception>
#include <system_error>

using namespace smlp;

smlp::Result Manager::predict(const std::string &line) {
  // no log here as the output is the result
  if (!predict_) {
    predict_ = std::make_unique<Predict>();
  }
  return predict_->predict(line);
}

smlp::Result Manager::train(const std::string &line) {
  auto handleStdinTraining = [this]() {
    logger.info("Training, using command pipe input...");
    if (app_params.training_ratio_line == 0 || app_params.num_epochs > 1) {
      logger.warn("Epochs and training ratio are disabled using command "
                  "pipe input. Use training_ratio_line parameter instead.");
    }
    logger.info(getInlineHeader());
  };

  auto handleFileTraining = [this]() {
    logger.info("Training, using file ", app_params.data_file);
    logger.info(getInlineHeader());
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
      logger.debug("Training using TCP socket...");
    }
    break;
  default:
    break;
  }

  return training_->train(line);
}

smlp::Result Manager::test(const std::string &line) {
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
      logger.debug("Testing using TCP socket...");
    }
    break;
  default:
    break;
  }

  const auto &results = testing_->test(line);
  if (app_params.input == EInput::Socket && app_params.verbose) {
    logger.debug(testing_->getTestingResults()->getResultsLine());
  } else {
    logger.out(testing_->getTestingResults()->getResultsDetailled());
  }
  return results;
}

smlp::Result Manager::trainTestMonitored(const std::string &line) {
  if (app_params.output_index_to_monitor > network_params.output_size) {
    logger.error("output_index_to_monitor > output_size: ",
                 app_params.output_index_to_monitor, ">",
                 network_params.output_size);
    return {.code = std::make_error_code(std::errc::result_out_of_range)};
  }

  if (app_params.verbose) {
    switch (app_params.input) {
    case EInput::File:
      logger.debug("Train and testing, using file ", app_params.data_file);
      logger.debug("OutputIndexToMonitor:", app_params.output_index_to_monitor,
                   " ", getInlineHeader());
      break;
    case EInput::Stdin:
      logger.debug("Train and testing, using command pipe input...");
      logger.debug("OutputIndexToMonitor:", app_params.output_index_to_monitor,
                   " ", getInlineHeader());
      break;
    case EInput::Socket:
      logger.debug("Train and testing using TCP socket...");
      break;
    default:
      break;
    }
  }

  if (!training_) {
    createTraining();
  }
  return training_->train(line);
}

std::string Manager::getVersionHeader() const {
  std::stringstream sst;
  sst << app_params.title << " v" << app_params.version;
  return sst.str();
}

std::string Manager::getInlineHeader() const {
  // lambda helper
  auto getActivationParams = [](const std::string &prefix,
                                EActivationFunction activation_function,
                                double activation_alpha) {
    std::stringstream sst;
    sst << " " << prefix
        << "ActivationFunction:" << smlp::getActivationStr(activation_function);
    if (activation_function == EActivationFunction::ELU ||
        activation_function == EActivationFunction::PReLU) {
      sst << " " << prefix << "ActivationAlpha:" << activation_alpha;
    }
    return sst.str();
  };
  // lambda helper
  auto getTrainingParams = [](const AppParameters &app_parameters) {
    std::stringstream sst;
    if (app_parameters.input != EInput::Stdin) {
      sst << " Epochs:" << app_parameters.num_epochs;
      if (app_parameters.training_ratio_line > 0) {
        sst << " TrainingRatioLine:" << app_parameters.training_ratio_line;
      } else {
        sst << " TrainingRatio:" << app_parameters.training_ratio;
      }
    } else {
      sst << " TrainingRatioLine:" << app_parameters.training_ratio_line;
    }
    return sst.str();
  };

  std::stringstream sst;
  sst << "InputSize:" << network_params.input_size
      << " OutputSize:" << network_params.output_size
      << " HiddenSize:" << network_params.hidden_size
      << " HiddenLayers:" << network_params.hiddens_count
      << " LearningRate:" << network_params.learning_rate
      << getActivationParams("Hidden",
                             network_params.hidden_activation_function,
                             network_params.hidden_activation_alpha)
      << getActivationParams("Output",
                             network_params.output_activation_function,
                             network_params.output_activation_alpha)
      << getTrainingParams(app_params)
      << " Mode:" << smlp::getModeStr(app_params.mode)
      << " Verbose:" << app_params.verbose;
  return sst.str();
}

void Manager::runMode() {
  switch (app_params.input) {
  case EInput::File:
    runWithVisitor(RunnerFileVisitor{});
    break;
  case EInput::Stdin:
    runWithVisitor(RunnerStdinVisitor{});
    break;
  case EInput::Socket:
    runWithVisitor(RunnerSocketVisitor{});
    break;
  default:
    break;
  }
  // TODO: remove code below, replace it with visitors above.
  //  HTTP service mode
  if (app_params.enable_http) {
    createHttpServer();
    if (!http_server) {
      throw ManagerException(SimpleLang::Error(Error::InternalError));
    }
    http_server->start();
  } else {
    // Terminal modes
    switch (app_params.mode) {
    case EMode::Predict:
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
    if (shouldExportNetwork()) {
      exportNetwork();
    }
  }
}

void Manager::runWithVisitor(const RunnerVisitor &visitor) { visitor.visit(); }

void Manager::importOrBuildNetwork() {
  // avoiding header lines on Predict mode, for commands chaining with
  // pipes
  auto logNetworkImport = [this]() {
    if (app_params.mode != EMode::Predict) {
      logger.info("Importing network model from ", app_params.network_to_import,
                  "...");
    }
  };
  auto logNetworkCreation = [this]() {
    if (!app_params.network_to_import.empty() &&
        app_params.mode != EMode::Predict) {
      logger.info("Network model ", app_params.network_to_import,
                  " not found, creating a new one...");
    }
  };

  if (!app_params.network_to_import.empty() &&
      std::filesystem::exists(app_params.network_to_import)) {
    logNetworkImport();
    network = importExport.importModel(app_params);
  } else {
    logNetworkCreation();
    network = std::make_unique<Network>();
    network->initializeLayers();
  }
}

bool Manager::shouldExportNetwork() const {
  return !app_params.network_to_export.empty() &&
         (app_params.mode == EMode::TrainOnly ||
          app_params.mode == EMode::TrainThenTest ||
          app_params.mode == EMode::TrainTestMonitored);
}

void Manager::exportNetwork() {
  if (app_params.network_to_export.empty()) {
    throw ManagerException(SimpleLang::Error(Error::MissingExportFileParam));
  }
  logger.info("Exporting network model to ", app_params.network_to_export,
              "...");
  importExport.exportModel(network, app_params);
}

smlp::Result Manager::processTCPClient(const std::string &line) {
  if (app_params.input != EInput::Socket) {
    throw ManagerException(SimpleLang::Error(Error::TCPSocketNotSet));
  }
  smlp::Result result;
  switch (app_params.mode) {
  case EMode::Predict:
    result = predict(line);
    break;
  case EMode::TrainOnly:
    result = train(line);
    break;
  case EMode::TestOnly:
    result = test(line);
    break;
  case EMode::TrainTestMonitored:
    result = trainTestMonitored(line);
    break;
  case EMode::TrainThenTest:
    train(line);
    result = test(line);
    break;
  default:
    throw ManagerException(SimpleLang::Error(Error::UnimplementedMode));
  }
  result.action = smlp::getModeStr(app_params.mode);
  return result;
}