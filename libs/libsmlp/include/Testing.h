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
#include "FileParser.h"
#include "Network.h"
#include <cstddef>
#include <map>

class Testing : public FileParser {
public:
  Testing(Network *network, const std::string &file_path)
      : FileParser(file_path), network_(network) {}

  /**
   * @brief Test the neural network with some data
   *
   * @param ouput_at_end if true the output value must be at end of record, else
   * at start.
   * @param from_line start train from this line, must be lesser than total
   * lines.
   * @param to_line end train at this line (including), must be greater than
   * from_line or equal to 0 for end of file.
   * @return bool true if success
   */
  bool Test(bool output_at_end = true, size_t from_line = 0, size_t to_line = 0,
            size_t epoch = 0);

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
  std::map<int, std::vector<float>> progress;

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
};