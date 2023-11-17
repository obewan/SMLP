#include "Training.h"
#include "Common.h"
#include "SimpleLogger.h"
#include <chrono>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_view_literals;

void Training::train(const NetworkParameters &network_params,
                     const AppParameters &app_params) {

  auto processInputLine = [this, &network_params, &app_params](auto &line) {
    RecordResult result =
        fileParser_->processLine(network_params, app_params, line);
    if (result.isSuccess) {
      network_->forwardPropagation(result.record.first);
      network_->backwardPropagation(result.record.second);
      network_->updateWeights();
    }
  };

  if (app_params.use_stdin) {
    // Read from stdin
    std::string line;
    while (std::getline(std::cin, line)) {
      processInputLine(line);
    }
  } else {
    // Read from file
    if (!fileParser_->isTrainingRatioLineProcessed) {
      fileParser_->getTrainingRatioLine(app_params.training_ratio,
                                        app_params.training_ratio_line);
    }
    if (fileParser_->training_ratio_line == 0) {
      throw TrainingException("invalid parameter: training_ratio is too small, "
                              "no data for training.");
    }
    fileParser_->openFile();
    for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
      logger_.info("Training epoch ", epoch + 1, "/", app_params.num_epochs,
                   "... ");
      fileParser_->resetPos();
      for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
        processInputLine("");
      }
    }
    fileParser_->closeFile();
  }
}

void Training::trainTestMonitored(const NetworkParameters &network_params,
                                  const AppParameters &app_params) {

  if (!fileParser_->isTrainingRatioLineProcessed) {
    fileParser_->getTrainingRatioLine(app_params.training_ratio,
                                      app_params.training_ratio_line);
  }
  if (fileParser_->training_ratio_line == 0) {
    throw TrainingException("invalid parameter: training_ratio is too small, "
                            "no data for training.");
  }
  fileParser_->openFile();
  auto testing = std::make_shared<Testing>(network_, fileParser_);
  setTesting(testing);

  const auto start{std::chrono::steady_clock::now()};
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    logger_.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
                app_params.num_epochs, "... ");
    fileParser_->resetPos();
    for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
      RecordResult result =
          fileParser_->processLine(network_params, app_params);
      if (result.isSuccess) {
        network_->forwardPropagation(result.record.first);
        network_->backwardPropagation(result.record.second);
        network_->updateWeights();
      }
    }

    logger_.append("testing... ");
    testing_->test(network_params, app_params, epoch);
    logger_.out(testing_->showResultsLine());
  }
  const auto end{std::chrono::steady_clock::now()};

  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger_.info("Elapsed time: ", elapsed_seconds.count(), "s");
  logger_.info(testing_->showResults(app_params.mode, app_params.verbose));

  fileParser_->closeFile();
}
