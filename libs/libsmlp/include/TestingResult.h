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
#include "CommonParameters.h"
#include <cstddef>
#include <vector>

class TestingResult {
public:
  /**
   * @brief Construct a new Testing Result object
   *
   * @param app_params
   */
  explicit TestingResult(const AppParameters &app_params)
      : app_params_(app_params){};

  struct Convergence {
    float previous = 0.0f;
    float current = 0.0f;
    float expected = 0.0f;
    bool hasPrevious = false;
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
    size_t correct_predictions_avg = 0;
    size_t correct_predictions_high = 0;
    size_t good_convergence = 0;
    size_t good_convergence_zero = 0;
    size_t good_convergence_one = 0;
    size_t total_expected_zero = 0;
    size_t total_expected_one = 0;
    size_t total_convergences = 0;

    float accuracy_avg = 0.0f;
    float accuracy_low = 0.0f;
    float accuracy_high = 0.0f;
    float convergence = 0.0f;
    float convergence_zero = 0.0f;
    float convergence_one = 0.0f;

    bool isCalculate = false;
  };

  /**
   * @brief Displays a summary of the test results on a single line.
   */
  std::string showResultsLine();

  /**
   * @brief Displays detailed test results.
   *
   */
  std::string showDetailledResults();

  /**
   * @brief Calculates the statistics of the test results
   */
  void calcStats();

  /**
   * @brief Get the Stats object
   *
   * @return const TestingResult::Stat&
   */
  const TestingResult::Stat &getStats() const { return stats; }

  /**
   * @brief process the results to get the training progress
   *
   * @param testResult
   */
  void processRecordTestingResult(const TestingResult::TestResults &testResult);

  /**
   * @brief Get the Progress object
   *
   * @return const std::map<size_t, Convergence>&
   */
  const std::map<size_t, Convergence> &getProgress() const { return progress; }

  /**
   * @brief clear stats
   *
   */
  void clearStats() {
    stats.convergence = 0.f;
    stats.convergence_one = 0.f;
    stats.convergence_zero = 0.f;
    stats.good_convergence_zero = 0;
    stats.good_convergence = 0;
    stats.good_convergence_one = 0;
    stats.total_convergences = 0;
    stats.accuracy_avg = 0.f;
    stats.accuracy_high = 0.f;
    stats.accuracy_low = 0.f;
    stats.correct_predictions_avg = 0;
    stats.correct_predictions_high = 0;
    stats.correct_predictions_low = 0;
    stats.total_expected_one = 0;
    stats.total_expected_zero = 0;
    stats.total_samples = 0;
    stats.isCalculate = false;
  }

private:
  std::string showAccuracyResults() const;
  std::string showConvergenceResults() const;
  void updateMonitoredProgress(const TestingResult::TestResults &result);
  void calculateConvergences();
  void calculatePercentages();

  std::map<size_t, Convergence> progress;
  TestingResult::Stat stats;
  const AppParameters &app_params_;
};