#include "Training.h"
#include "Common.h"
#include <chrono>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_view_literals;

void Training::train(const NetworkParameters &params) {
  if (!fileParser_->isTrainingRatioLineProcessed) {
    fileParser_->getTrainingRatioLine(params.training_ratio);
  }
  if (fileParser_->training_ratio_line == 0) {
    throw TrainingException("invalid parameter: training_ratio is too small, "
                            "no data for training.");
  }
  fileParser_->openFile();
  for (size_t epoch = 0; epoch < params.num_epochs; epoch++) {
    std::cout << "[INFO] Training epoch " << epoch + 1 << "/"
              << params.num_epochs << "... " << std::endl;
    fileParser_->resetPos();
    for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
      RecordResult result = fileParser_->processLine(params);
      if (result.isSuccess) {
        network_->forwardPropagation(result.record.first);
        network_->backwardPropagation(result.record.second);
        network_->updateWeights();
      }
    }
  }
  fileParser_->closeFile();
}

void Training::trainTestMonitored(const NetworkParameters &params) {

  if (!fileParser_->isTrainingRatioLineProcessed) {
    fileParser_->getTrainingRatioLine(params.training_ratio);
  }
  if (fileParser_->training_ratio_line == 0) {
    throw TrainingException("invalid parameter: training_ratio is too small, "
                            "no data for training.");
  }
  fileParser_->openFile();
  setTesting(new Testing(network_, fileParser_));

  const auto start{std::chrono::steady_clock::now()};
  for (size_t epoch = 0; epoch < params.num_epochs; epoch++) {
    std::cout << "[INFO] Training epoch " << epoch + 1 << "/"
              << params.num_epochs << "... ";
    fileParser_->resetPos();
    for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
      RecordResult result = fileParser_->processLine(params);
      if (result.isSuccess) {
        network_->forwardPropagation(result.record.first);
        network_->backwardPropagation(result.record.second);
        network_->updateWeights();
      }
    }

    std::cout << "testing... ";
    testing_->test(params, epoch);
    testing_->showResultsLine();
    std::cout << std::endl;
  }
  const auto end{std::chrono::steady_clock::now()};

  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
  testing_->showResults(params.verbose);

  fileParser_->closeFile();
}
