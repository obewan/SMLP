#pragma once
#include "Training.h"

class TrainingStdin : public Training {
public:
  TrainingStdin(const NetworkParameters &network_params,
                const AppParameters &app_params)
      : Training(TrainingType::TrainingStdin, network_params, app_params) {}

  void train(const std::string &line = "") override {
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
      testing_->test();
      logger_.out(testing_->getTestingResults()->showResultsLine());
    }
  }
};