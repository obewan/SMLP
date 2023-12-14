/**
 * @file TestingResult.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingResult
 * @date 2023-11-17
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include <cstddef>
#include <vector>

class TestingResult {
public:
  /**
   * @brief TestResult structure that holds the epoch, line, expected output,
   * and actual output of a test, on a specific monitored neuron.
   */
  struct TestResult {
    size_t epoch = 0;
    size_t lineNumber = 0;
    float expected = 0.0f;
    float output = 0.0f;
    std::vector<float> progress; // This will be empty for unmonitored data
  };

  /**
   * @brief TestResults structure that holds the epoch, line, expected outputs,
   * and actual outputs of a test, for all output neurons (not monitored).
   */
  struct TestResults {
    size_t epoch = 0;
    size_t lineNumber = 0;
    std::vector<float> expecteds;
    std::vector<float> outputs;
  };

  /**
   * @brief Stat structure that holds various statistics related to the test
   * results, such as total samples, correct predictions, good convergence,
   * total expected values, and accuracy.
   */
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

  /**
   * @brief Displays a summary of the test results on a single line.
   */
  std::string showResultsLine(bool withConvergence);

  /**
   * @brief Displays detailed test results. If the verbose parameter is set to
   * true, additional information will be displayed.
   *
   * @param mode Mode of the training.
   * @param verbose If true, additional details are displayed (default is
   * false).
   */
  std::string showDetailledResults(EMode mode, bool verbose = false);

  /**
   * @brief Display some verbose results.
   *
   */
  std::string showResultsVerbose(const TestResult &result, EMode mode) const;

  /**
   * @brief Calculates and returns the statistics of the test results, including
   * monitored progress if monitored is true.
   *
   * @param monitored If true, return stats with progress
   * @return A Stat object containing the calculated statistics.
   */
  Stat calcStats(bool monitored);

  /**
   * @brief process the results to get the training progress
   *
   * @param testResult
   */
  void processRecordTestingResult(const TestingResult::TestResults &testResult);

  /**
   * @brief process the results to get the training progress
   *
   * @param testResults
   * @param mode
   * @param last_epoch
   */
  void processResults(const std::vector<TestingResult::TestResult> &testResults,
                      EMode mode, size_t last_epoch = 0);

  const std::map<size_t, std::vector<float>> &getProgress() const {
    return progress;
  }

private:
  std::string showAccuracyResults(const TestingResult::Stat &stats) const;
  std::string showConvergenceResults(const TestingResult::Stat &stats) const;
  void updateStats(TestingResult::Stat &stats, const TestResult &result,
                   bool monitored);
  void updateConvergenceStats(Stat &stats, const TestResult &result) const;
  void updatePredictionStats(Stat &stats, const TestResult &result) const;
  void calculatePercentages(Stat &stats, bool monitored) const;

  std::vector<TestResult> testResultExts;
  std::map<size_t, std::vector<float>> progress;
  std::vector<TestResult> lastEpochTestResultTemp_;
  size_t last_epoch_ = 0;
};