/**
 * @file TestingStdin.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingStdin
 * @date 2023-12-14
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Testing.h"
#include <cstddef>
#include <iostream>

class TestingStdin : public Testing {
public:
  using Testing::Testing;

  void test(size_t epoch = 0, size_t current_line = 0) override {
    std::string line;
    if (app_params_.mode == EMode::TrainThenTest ||
        app_params_.use_training_ratio_line) {
      current_line = app_params_.training_ratio_line;
    }

    auto network_params = network_->params;

    while (std::getline(std::cin, line)) {
      fileParser_->current_line_number = current_line - 1;
      RecordResult result =
          fileParser_->processLine(network_params, app_params_, line);
      if (!result.isSuccess) {
        continue;
      }
      auto testResult = testLine(result, fileParser_->current_line_number);
      current_line++;
      testingResults_->processRecordTestingResult(testResult);
    }
  }
};