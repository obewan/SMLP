#include "Testing.h"
#include "Common.h"
#include "SimpleLang.h"
#include "TestingResult.h"
#include "exception/TestingException.h"
#include <cstddef>
#include <iostream>

using namespace std::string_view_literals;

void Testing::test(const NetworkParameters &network_params,
                   const AppParameters &app_params, size_t epoch,
                   size_t current_line) {
  if (app_params.use_stdin) {
    testFromStdin(network_params, app_params, current_line);
  } else {
    testFromFile(network_params, app_params, epoch);
  }
}

void Testing::testFromFile(const NetworkParameters &network_params,
                           const AppParameters &app_params,
                           size_t epoch) const {
  if (app_params.mode == EMode::TrainThenTest ||
      app_params.use_training_ratio_line) {
    if (!fileParser_->isTrainingRatioLineProcessed) {
      fileParser_->getTrainingRatioLine(app_params.training_ratio,
                                        app_params.training_ratio_line,
                                        app_params.use_stdin);
    }
    if (fileParser_->training_ratio_line >= fileParser_->total_lines) {
      throw TestingException(SimpleLang::Error(Error::InvalidTrainingRatio));
    }
  }

  if (!fileParser_->file.is_open()) {
    fileParser_->openFile();
  }

  std::vector<TestingResult::TestResults> testResults;
  bool isTesting = true;
  int output_neuron_to_monitor = (int)app_params.output_index_to_monitor - 1;
  bool isValidOutputNeuronToMonitor =
      output_neuron_to_monitor >= 0 &&
      output_neuron_to_monitor < (int)network_params.output_size;
  while (isTesting) {
    RecordResult result = fileParser_->processLine(network_params, app_params);
    if (result.isSuccess) {
      auto predicteds = network_->forwardPropagation(result.record.first);
      // Using just one output neuron to monitor, or else there will be too much
      // memory used.
      if (isValidOutputNeuronToMonitor) {
        testResults.emplace_back(epoch, fileParser_->current_line_number,
                                 result.record.second[output_neuron_to_monitor],
                                 predicteds[output_neuron_to_monitor]);
      }
    } else {
      isTesting = false;
    }
  }

  testingResults_->processResults(testResults, app_params.mode, epoch);
}

void Testing::testFromStdin(const NetworkParameters &network_params,
                            const AppParameters &app_params,
                            size_t current_line) const {
  std::string line;
  if (app_params.mode == EMode::TrainThenTest ||
      app_params.use_training_ratio_line) {
    current_line = app_params.training_ratio_line;
  }
  std::vector<TestingResult::TestResults> testResults;

  while (std::getline(std::cin, line)) {
    fileParser_->current_line_number = current_line - 1;
    RecordResult result =
        fileParser_->processLine(network_params, app_params, line);
    testLine(network_params, app_params, result, current_line, testResults);
    current_line++;
  }
  testingResults_->processResults(testResults, app_params.mode);
}

void Testing::testLine(
    const NetworkParameters &network_params, const AppParameters &app_params,
    const RecordResult &record_result, const size_t line_number,
    std::vector<TestingResult::TestResults> &testResults) const {
  if (!record_result.isSuccess) {
    return;
  }
  int output_neuron_to_monitor = (int)app_params.output_index_to_monitor - 1;
  auto predicteds = network_->forwardPropagation(record_result.record.first);
  // Using just one output neuron to monitor, or else there will be too much
  // memory used.
  if (output_neuron_to_monitor >= 0 &&
      output_neuron_to_monitor < (int)network_params.output_size) {
    testResults.emplace_back(
        0, line_number, record_result.record.second[output_neuron_to_monitor],
        predicteds[output_neuron_to_monitor]);
  }
}
