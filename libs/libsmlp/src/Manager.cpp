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
    if (app_params.enable_http) {
      createHttpServer();
      if (!http_server) {
        throw ManagerException(SimpleLang::Error(Error::InternalError));
      }
      http_server->start();
    }
    break;
  default:
    break;
  }
}

void Manager::runWithVisitor(const RunnerVisitor &visitor) { visitor.visit(); }

Result Manager::runWithLineVisitor(const RunnerVisitor &visitor,
                                   const std::string &line) {
  return visitor.visit(line);
}

void Manager::importOrBuildNetwork() {
  // avoiding header lines on Predict mode, for commands chaining with
  // pipes
  auto logNetworkImport = [this]() {
    if (app_params.mode != EMode::Predict) {
      SimpleLogger::LOG_INFO("Importing network model from ",
                             app_params.network_to_import, "...");
    }
  };
  auto logNetworkCreation = [this]() {
    if (!app_params.network_to_import.empty() &&
        app_params.mode != EMode::Predict) {
      SimpleLogger::LOG_INFO("Network model ", app_params.network_to_import,
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

void Manager::exportNetwork() const {
  if (app_params.network_to_export.empty()) {
    throw ManagerException(SimpleLang::Error(Error::MissingExportFileParam));
  }
  SimpleLogger::LOG_INFO("Exporting network model to ",
                         app_params.network_to_export, "...");
  importExport.exportModel(network, app_params);
}

smlp::Result Manager::processTCPClient(const std::string &line) {
  if (app_params.input != EInput::Socket) {
    throw ManagerException(SimpleLang::Error(Error::TCPSocketNotSet));
  }
  return runWithLineVisitor(RunnerSocketVisitor{}, line);
}