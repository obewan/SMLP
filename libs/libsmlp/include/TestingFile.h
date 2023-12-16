/**
 * @file TestingFile.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingFile
 * @date 2023-12-14
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonModes.h"
#include "SimpleLang.h"
#include "Testing.h"
#include "exception/TestingException.h"
#include <memory>

class TestingFile : public Testing {
public:
  explicit TestingFile(const AppParameters &app_params) : Testing(app_params) {}

  void test(const std::string &line = "", size_t epoch = 0,
            size_t current_line_number = 0) override {
    testingResults_->clearStats();
    if (!fileParser_ || !network_) {
      throw TestingException(SimpleLang::Error(Error::InternalError));
    }
    if (app_params_.mode == EMode::TrainThenTest ||
        app_params_.use_training_ratio_line) {
      if (!fileParser_->isTrainingRatioLineProcessed) {
        fileParser_->calcTrainingRatioLine(app_params_);
      }
      if (fileParser_->training_ratio_line >= fileParser_->total_lines) {
        throw TestingException(
            SimpleLang::Error(Error::InvalidTrainingRatioTooBig));
      }
    }

    if (!fileParser_->file.is_open()) {
      fileParser_->openFile();
    }

    auto network_params = network_->params;
    while (true) {
      RecordResult result =
          fileParser_->processLine(network_params, app_params_);
      if (!result.isSuccess) {
        break;
      }
      auto testResult =
          testLine(result, fileParser_->current_line_number, epoch);
      testingResults_->processRecordTestingResult(testResult);
    }
  }
};