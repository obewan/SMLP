#pragma once
#include "Training.h"

class TrainingStdin : public Training {
public:
  using Training::Training;
  virtual ~TrainingStdin() = default;

  void train(const std::string &line = "") override {
    if (app_params_.mode == EMode::TrainTestMonitored) {
      createTesting();
    }
    logger_.log(LogLevel::INFO, false, "Training...");
    size_t current_line = 0;
    std::string lineIn;
    while (std::getline(std::cin, lineIn) &&
           (fileParser_->training_ratio_line == 0 ||
            current_line < fileParser_->training_ratio_line)) {
      processInputLine(lineIn);
      current_line++;
    }
    if (app_params_.mode == EMode::TrainTestMonitored) {
      logger_.append("testing... ");
      testing_->testFromStdin(network_params_, app_params_, current_line);
      logger_.out(testing_->getTestingResults()->showResultsLine(false));
    }
  }
};