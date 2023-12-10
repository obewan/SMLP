#include "Manager.h"
#include "Predict.h"
#include "Training.h"
#include "exception/ManagerException.h"

void Manager::predict() {
  auto predict = std::make_unique<Predict>(network, app_params);
  predict->predict();
}

void Manager::train() {
  const auto &logger = SimpleLogger::getInstance();
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

void Manager::test() {
  const auto &logger = SimpleLogger::getInstance();
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

void Manager::trainTestMonitored() {
  const auto &logger = SimpleLogger::getInstance();
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
  const auto &logger = SimpleLogger::getInstance();
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

bool Manager::shouldExportNetwork() const {
  return !app_params.network_to_export.empty() &&
         (app_params.mode == EMode::TrainOnly ||
          app_params.mode == EMode::TrainTestMonitored ||
          app_params.mode == EMode::TrainThenTest);
}

void Manager::exportNetwork() {
  const auto &logger = SimpleLogger::getInstance();
  logger.info("Exporting network model to ", app_params.network_to_export,
              "...");
  importExportJSON.exportModel(network.get(), app_params);
}