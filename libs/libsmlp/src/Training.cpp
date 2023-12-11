#include "Training.h"
#include "Common.h"
#include "SimpleLogger.h"
#include "TestingResult.h"
#include <chrono>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_view_literals;

void Training::train(const NetworkParameters &network_params,
                     const AppParameters &app_params, const std::string &line) {
  if (app_params.use_socket) {
    processInputLine(network_params, app_params, line);
  } else if (app_params.use_stdin) {
    trainFromStdin(network_params, app_params);
  } else {
    trainFromFile(network_params, app_params);
  }
}

void Training::trainFromStdin(const NetworkParameters &network_params,
                              const AppParameters &app_params) {
  const auto &logger = SimpleLogger::getInstance();
  if (app_params.mode == EMode::TrainTestMonitored) {
    createTesting();
  }
  logger.log(LogLevel::INFO, false, "Training...");
  size_t current_line = 0;
  std::string line;
  while (std::getline(std::cin, line) &&
         (fileParser_->training_ratio_line == 0 ||
          current_line < fileParser_->training_ratio_line)) {
    processInputLine(network_params, app_params, line);
    current_line++;
  }
  if (app_params.mode == EMode::TrainTestMonitored) {
    logger.append("testing... ");
    testing_->testFromStdin(network_params, app_params, current_line);
    logger.out(testing_->getTestingResults()->showResultsLine(false));
  }
}

void Training::trainFromFile(const NetworkParameters &network_params,
                             const AppParameters &app_params) {
  const auto &logger = SimpleLogger::getInstance();
  if (!fileParser_->isTrainingRatioLineProcessed) {
    fileParser_->calcTrainingRatioLine(app_params);
  }
  if (fileParser_->training_ratio_line == 0) {
    throw TrainingException(
        SimpleLang::Error(Error::InvalidTrainingRatioTooSmall));
  }
  if (app_params.mode == EMode::TrainTestMonitored) {
    createTesting();
  }
  fileParser_->openFile();
  const auto start{std::chrono::steady_clock::now()};
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    logger.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
               app_params.num_epochs, "... ");
    fileParser_->resetPos();
    for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
      processInputLine(network_params, app_params);
    }
    if (app_params.mode == EMode::TrainTestMonitored) {
      logger.append("testing... ");
      testing_->testFromFile(network_params, app_params, epoch);
      logger.out(testing_->getTestingResults()->showResultsLine(
          app_params.mode == EMode::TrainTestMonitored));
    } else {
      logger.endl();
    }
  }
  const auto end{std::chrono::steady_clock::now()};
  fileParser_->closeFile();

  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger.info("Elapsed time: ", elapsed_seconds.count(), "s");
  if (app_params.mode == EMode::TrainTestMonitored) {
    logger.info(testing_->getTestingResults()->showDetailledResults(
        app_params.mode, app_params.verbose));
  }
}

RecordResult Training::processInputLine(const NetworkParameters &network_params,
                                        const AppParameters &app_params,
                                        const std::string &line) const {
  RecordResult result =
      fileParser_->processLine(network_params, app_params, line);
  if (result.isSuccess) {
    network_->forwardPropagation(result.record.first);
    network_->backwardPropagation(result.record.second);
    network_->updateWeights();
  }
  return result;
}