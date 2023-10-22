/**
 * @file Testing.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Testing
 * @date 2023-08-29
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "FileParser.h"
#include "Network.h"
#include <cstddef>
#include <map>

class Testing {
public:
  Testing(Network *network, const std::string &file_path)
      : network_(network), fileParser_(file_path) {}

  void test(const Parameters &params, size_t epoch = 0);

  struct TestResults {
    size_t epoch = 0;
    size_t line = 0;
    float expected = 0.0f;
    float output = 0.0f;
  };

  struct TestResultsExt {
    size_t epoch = 0;
    size_t line = 0;
    float expected = 0.0f;
    float output = 0.0f;
    std::vector<float> progress;
  };

  std::vector<TestResultsExt> testResultExts;
  std::map<size_t, std::vector<float>> progress;

  struct Stat {
    size_t total_samples = 0;
    size_t correct_predictions_low = 0;
    size_t correct_predictions = 0;
    size_t correct_predictions_high = 0;
    size_t good_convergence = 0;
    size_t good_convergence_zero = 0;
    size_t good_convergence_one = 0;
    size_t total_expected_zero = 0;
    size_t total_expected_one = 0;

    float accuracy = 0.0f;
    float accuracy_low = 0.0f;
    float accuracy_high = 0.0f;
    float convergence = 0.0f;
    float convergence_zero = 0.0f;
    float convergence_one = 0.0f;
  };

  void showResultsLine();
  void showResults(bool verbose = false);
  Stat calcStats();

private:
  Network *network_;
  std::vector<TestResults> lastEpochTestResultTemp_;
  size_t last_epoch_ = 0;
  FileParser fileParser_;
};