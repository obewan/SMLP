#include "Testing.h"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>

using namespace std::string_view_literals;

bool Testing::Test(bool output_at_end, size_t from_line, size_t to_line,
                   size_t epoch) {
  if (from_line > to_line && to_line > 0) {
    std::cerr << "[ERROR] from_line is greater than to_line." << std::endl;
    return false;
  }

  if (!OpenFile()) {
    return false;
  }

  // Loop through the test data and evaluate the network's performance
  size_t input_size = network_->inputLayer.neurons.size();
  size_t ouput_size = network_->outputLayer.neurons.size();
  std::vector<TestResults> testResults;

  RecordFunction recordFunction =
      [&network = network_, &testResults](
          size_t stepoch, size_t stline,
          std::pair<std::vector<float>, std::vector<float>> const &record) {
        auto predicteds = network->forwardPropagation(record.first);
        testResults.emplace_back(stepoch, stline, record.second[0],
                                 predicteds[0]);
      };

  try {
    if (!ProcessFile(epoch, from_line, to_line, input_size, ouput_size,
                     output_at_end, recordFunction)) {
      std::cerr << "[ERROR] Error during file processing." << std::endl;
      return false;
    }
  } catch (std::exception &ex) {
    std::cerr << "[ERROR] Exception during file processing: " << ex.what()
              << std::endl;
    return false;
  }

  // Process Results
  for (auto const &result : testResults) {
    if (!progress.contains(result.line)) {
      progress[result.line] = {result.output};
    } else {
      progress.at(result.line).push_back(result.output);
    }
  }

  lastEpochTestResultTemp_ = testResults;
  last_epoch_ = epoch;

  CloseFile();

  return true;
}

void Testing::showResults() {
  for (auto const &result : lastEpochTestResultTemp_) {
    testResultExts.push_back({result.epoch, result.line, result.expected,
                              result.output, progress.at(result.line)});
  }

  size_t total_samples = testResultExts.size();
  size_t correct_predictions = 0;
  size_t good_convergence = 0;
  size_t good_convergence_zero = 0;
  size_t good_convergence_one = 0;
  size_t total_expected_zero = 0;
  size_t total_expected_one = 0;

  for (auto const &result : testResultExts) {
    std::cout << "Expected:" << result.expected
              << " Predicted:" << result.output << " [ ";
    for (auto &progres : result.progress) {
      std::cout << progres << " ";
    }
    std::cout << "] (" << result.progress.back() - result.progress.front()
              << ")" << std::endl;

    if (result.expected == 1) {
      total_expected_one++;
    } else {
      total_expected_zero++;
    }

    if (result.progress.size() > 1) {
      if (result.expected == 1 &&
          result.progress.back() > result.progress.front()) {
        good_convergence_one++;
        good_convergence++;
      } else if (result.expected == 0 &&
                 result.progress.back() < result.progress.front()) {
        good_convergence_zero++;
        good_convergence++;
      }
    }

    if (abs(result.expected - result.output) < 0.1) {
      correct_predictions++;
    }
  }
  float accuracy = 0;
  float convergence = 0;
  float convergence_zero = 0;
  float convergence_one = 0;
  if (total_samples > 0) {
    accuracy =
        static_cast<float>(correct_predictions) / (float)total_samples * 100.0f;
    convergence =
        static_cast<float>(good_convergence) / (float)total_samples * 100.0f;
    convergence_zero = static_cast<float>(good_convergence_zero) /
                       (float)total_expected_zero * 100.0f;
    convergence_one = static_cast<float>(good_convergence_one) /
                      (float)total_expected_one * 100.0f;
  }

  std::cout << std::endl;
  std::cout << std::setprecision(3) << "Accuracy: " << accuracy << "%"
            << std::endl;
  std::cout << std::setprecision(3)
            << "Good convergence toward zero: " << convergence_zero << "% ("
            << good_convergence_zero << "/" << total_expected_zero << ")"
            << std::endl;
  std::cout << std::setprecision(3)
            << "Good convergence toward one: " << convergence_one << "%  ("
            << good_convergence_one << "/" << total_expected_one << ")"
            << std::endl;
  std::cout << std::setprecision(3) << "Good convergence total: " << convergence
            << "% (" << good_convergence << "/" << total_samples << ")"
            << std::endl;
}