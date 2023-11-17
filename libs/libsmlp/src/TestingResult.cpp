#include "TestingResult.h"
#include <cmath>
#include <iomanip>
#include <iostream>

const float LOW_THRESHOLD = 0.30f;
const float MEDIUM_THRESHOLD = 0.20f;
const float HIGH_THRESHOLD = 0.10f;

std::string TestingResult::showResultsLine(bool withConvergence) {
  auto stats = calcStats(withConvergence);
  std::stringstream sstr;

  sstr << std::setprecision(2) << "acc(lah)[" << stats.accuracy_low << " "
       << stats.accuracy << " " << stats.accuracy_high << "]";
  if (withConvergence) {
    sstr << std::setprecision(2) << " conv(01t)[" << stats.convergence_zero
         << " " << stats.convergence_one << " " << stats.convergence << "]";
  }

  return sstr.str();
}

std::string TestingResult::showResultsVerbose(const TestResults &result,
                                              EMode mode) const {
  std::stringstream sstr;
  sstr << "Expected:" << result.expected << " Predicted:" << result.output;
  if (mode == EMode::TrainTestMonitored) {
    sstr << " [ ";
    for (auto &progres : result.progress) {
      sstr << progres << " ";
    }
    sstr << "] (" << result.progress.back() - result.progress.front() << ")";
  }
  return sstr.str();
}

std::string TestingResult::showDetailledResults(EMode mode, bool verbose) {
  auto stats = calcStats(mode == EMode::TrainTestMonitored);
  std::stringstream sstr;

  sstr << "Testing results: " << std::endl;

  if (verbose) {
    for (auto const &result : testResultExts) {
      sstr << showResultsVerbose(result, mode) << std::endl;
    }
    sstr << std::endl;
  }

  sstr << showAccuracyResults(stats);

  if (mode == EMode::TrainTestMonitored) {
    sstr << showConvergenceResults(stats);
  }
  return sstr.str();
}

std::string
TestingResult::showAccuracyResults(const TestingResult::Stat &stats) const {
  std::stringstream sstr;
  sstr << std::setprecision(3)
       << "Low accuracy (correct at 70%): " << stats.accuracy_low << "%"
       << std::endl
       << "Avg accuracy (correct at 80%): " << stats.accuracy << "%"
       << std::endl
       << "High accuracy (correct at 90%): " << stats.accuracy_high << "%"
       << std::endl;
  return sstr.str();
}

std::string
TestingResult::showConvergenceResults(const TestingResult::Stat &stats) const {
  std::stringstream sstr;
  sstr << std::setprecision(3)
       << "Good convergence toward zero: " << stats.convergence_zero << "% ("
       << stats.good_convergence_zero << "/" << stats.total_expected_zero << ")"
       << std::endl
       << "Good convergence toward one: " << stats.convergence_one << "%  ("
       << stats.good_convergence_one << "/" << stats.total_expected_one << ")"
       << std::endl
       << "Good convergence total: " << stats.convergence << "% ("
       << stats.good_convergence << "/" << stats.total_samples << ")"
       << std::endl;
  return sstr.str();
}

void TestingResult::processResults(
    const std::vector<TestingResult::TestResults> &testResults, EMode mode,
    size_t last_epoch) {
  if (mode == EMode::TrainTestMonitored) {
    // record the progress of an output neuron
    for (auto const &result : testResults) {
      if (!progress.contains(result.line)) {
        progress[result.line] = {result.output};
      } else {
        progress.at(result.line).push_back(result.output);
      }
    }
  }

  lastEpochTestResultTemp_ = testResults;
  last_epoch_ = last_epoch;
}

TestingResult::Stat TestingResult::calcStats(bool monitored) {
  if (monitored) {
    testResultExts.clear();
    for (auto const &result : lastEpochTestResultTemp_) {
      testResultExts.emplace_back(result.epoch, result.line, result.expected,
                                  result.output, progress.at(result.line));
    }
  }

  TestingResult::Stat stats;
  stats.total_samples =
      monitored ? testResultExts.size() : lastEpochTestResultTemp_.size();
  auto const &results = monitored ? testResultExts : lastEpochTestResultTemp_;

  if (stats.total_samples == 0) {
    return stats;
  }

  for (auto const &result : results) {
    updateStats(stats, result, monitored);
  }

  calculatePercentages(stats, monitored);

  return stats;
}

void TestingResult::updateStats(TestingResult::Stat &stats,
                                const TestResults &result, bool monitored) {
  if (result.expected == 1) {
    stats.total_expected_one++;
  } else {
    stats.total_expected_zero++;
  }

  if (monitored && result.progress.size() > 1) {
    updateConvergenceStats(stats, result);
  }

  updatePredictionStats(stats, result);
}

void TestingResult::updateConvergenceStats(Stat &stats,
                                           const TestResults &result) const {
  if (result.expected == 1 &&
      result.progress.back() > result.progress.front()) {
    stats.good_convergence_one++;
    stats.good_convergence++;
  } else if (result.expected == 0 &&
             result.progress.back() < result.progress.front()) {
    stats.good_convergence_zero++;
    stats.good_convergence++;
  }
}

void TestingResult::updatePredictionStats(Stat &stats,
                                          const TestResults &result) const {
  if (std::abs(result.expected - result.output) < LOW_THRESHOLD) {
    stats.correct_predictions_low++;
  }
  if (std::abs(result.expected - result.output) < MEDIUM_THRESHOLD) {
    stats.correct_predictions++;
  }
  if (std::abs(result.expected - result.output) < HIGH_THRESHOLD) {
    stats.correct_predictions_high++;
  }
}

void TestingResult::calculatePercentages(Stat &stats, bool monitored) const {
  stats.accuracy = static_cast<float>(stats.correct_predictions) /
                   (float)stats.total_samples * 100.0f;
  stats.accuracy_low = static_cast<float>(stats.correct_predictions_low) /
                       (float)stats.total_samples * 100.0f;
  stats.accuracy_high = static_cast<float>(stats.correct_predictions_high) /
                        (float)stats.total_samples * 100.0f;
  if (monitored) {
    stats.convergence = static_cast<float>(stats.good_convergence) /
                        (float)stats.total_samples * 100.0f;
    stats.convergence_zero = static_cast<float>(stats.good_convergence_zero) /
                             (float)stats.total_expected_zero * 100.0f;
    stats.convergence_one = static_cast<float>(stats.good_convergence_one) /
                            (float)stats.total_expected_one * 100.0f;
  }
}