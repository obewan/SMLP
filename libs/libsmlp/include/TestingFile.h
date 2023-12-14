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
#include "Testing.h"
#include "TestingResult.h"
#include <memory>

class TestingFile : Testing {
public:
  using Testing::Testing;
  virtual ~TestingFile() = default;

  void test(size_t epoch = 0) override {
    if (app_params.mode == EMode::TrainThenTest ||
        app_params.use_training_ratio_line) {
      if (!fileParser_->isTrainingRatioLineProcessed) {
        fileParser_->calcTrainingRatioLine(app_params);
      }
      if (fileParser_->training_ratio_line >= fileParser_->total_lines) {
        throw TestingException(
            SimpleLang::Error(Error::InvalidTrainingRatioTooBig));
      }
    }

    if (!fileParser_->file.is_open()) {
      fileParser_->openFile();
    }

    std::vector<TestingResult::TestResult> testResults;
    bool isTesting = true;
    auto output_neuron_to_monitor = app_params.output_index_to_monitor;
    auto network_params = network_->params;
    bool isValidOutputNeuronToMonitor =
        output_neuron_to_monitor < network_params.output_size;
    while (isTesting) {
      RecordResult result =
          fileParser_->processLine(network_params, app_params);
      if (result.isSuccess) {
        auto testResult =
            testLine(result, fileParser_->current_line_number, epoch);

        testingResults_->processRecordTestingResult(testResult);
        if (app_params.mode != EMode::TrainTestMonitored) {

        } else if (isValidOutputNeuronToMonitor) {
          testResults.emplace_back(
              epoch, fileParser_->current_line_number,
              result.record.outputs[output_neuron_to_monitor],
              testResult.outputs[output_neuron_to_monitor]);
        }
      } else {
        isTesting = false;
      }
    }

    testingResults_->processResults(testResults, app_params.mode, epoch);
  }

private:
  std::shared_ptr<TestingResult> testingResults_ =
      std::make_shared<TestingResult>();
};