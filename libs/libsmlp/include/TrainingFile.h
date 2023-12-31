/**
 * @file TrainingFile.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TrainingFile
 * @date 2023-12-12
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "SimpleLang.h"
#include "Training.h"
#include "exception/TrainingException.h"

class TrainingFile : public Training {
public:
  TrainingFile(const NetworkParameters &network_params,
               const AppParameters &app_params)
      : Training(TrainingType::TrainingFile, network_params, app_params) {}

  void train(const std::string &line = "") override {
    if (!fileParser_) {
      throw TrainingException(SimpleLang::Error(Error::InternalError));
    }
    if (!fileParser_->isTrainingRatioLineProcessed) {
      fileParser_->calcTrainingRatioLine(app_params_);
    }
    if (fileParser_->training_ratio_line == 0) {
      throw TrainingException(
          SimpleLang::Error(Error::InvalidTrainingRatioTooSmall));
    }
    fileParser_->openFile();
    const auto start{std::chrono::steady_clock::now()};
    for (size_t epoch = 0; epoch < app_params_.num_epochs; epoch++) {
      logger_.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
                  app_params_.num_epochs, "... ");
      fileParser_->resetPos();
      for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
        processInputLine();
      }
      if (app_params_.mode == EMode::TrainTestMonitored) {
        if (!testing_) {
          throw TrainingException(SimpleLang::Error(Error::InternalError));
        }
        logger_.append("testing... ");
        testing_->test("", epoch);
        logger_.out(testing_->getTestingResults()->showResultsLine());
      } else {
        logger_.endl();
      }
    }
    const auto end{std::chrono::steady_clock::now()};
    fileParser_->closeFile();

    const std::chrono::duration<double> elapsed_seconds{end - start};
    logger_.info("Elapsed time: ", elapsed_seconds.count(), "s");
    if (app_params_.mode == EMode::TrainTestMonitored) {
      logger_.info(testing_->getTestingResults()->showDetailledResults());
    }
  }
};