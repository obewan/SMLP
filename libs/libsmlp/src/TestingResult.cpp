#include "TestingResult.h"
#include "CommonModes.h"
#include "exception/TestingException.h"
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>

const float LOW_THRESHOLD = 0.30f;
const float MEDIUM_THRESHOLD = 0.20f;
const float HIGH_THRESHOLD = 0.10f;

std::string TestingResult::showResultsLine() {
  calcStats();
  std::stringstream sstr;

  sstr << std::setprecision(2) << "acc(lah)[" << stats.accuracy_low << " "
       << stats.accuracy_avg << " " << stats.accuracy_high << "]";
  if (app_params_.mode == EMode::TrainTestMonitored) {
    sstr << std::setprecision(2) << " conv(01t)[" << stats.convergence_zero
         << " " << stats.convergence_one << " " << stats.convergence << "]";
  }

  return sstr.str();
}

std::string TestingResult::showDetailledResults() {
  calcStats();
  std::stringstream sstr;

  sstr << "Testing results: " << std::endl;

  sstr << showAccuracyResults();

  if (app_params_.mode == EMode::TrainTestMonitored) {
    sstr << showConvergenceResults();
  }
  return sstr.str();
}

std::string TestingResult::showAccuracyResults() const {
  std::stringstream sstr;
  sstr << std::setprecision(3)
       << "Low accuracy (correct at 70%): " << stats.accuracy_low << "%"
       << std::endl
       << "Avg accuracy (correct at 80%): " << stats.accuracy_avg << "%"
       << std::endl
       << "High accuracy (correct at 90%): " << stats.accuracy_high << "%"
       << std::endl;
  return sstr.str();
}

std::string TestingResult::showConvergenceResults() const {
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

void TestingResult::processRecordTestingResult(
    const TestingResult::TestResults &testResult) {
  if (testResult.expecteds.size() != testResult.outputs.size()) {
    throw TestingException("invalid expecteds and outputs size.");
  }

  const size_t expectedsSize = testResult.expecteds.size();

  for (size_t i = 0; i < expectedsSize; ++i) {
    const auto error =
        std::abs(testResult.expecteds[i] - testResult.outputs[i]);

    if (testResult.expecteds[i] == 1) {
      stats.total_expected_one++;
    } else {
      stats.total_expected_zero++;
    }

    if (error < LOW_THRESHOLD) {
      stats.correct_predictions_low++;
    }
    if (error < MEDIUM_THRESHOLD) {
      stats.correct_predictions_avg++;
    }
    if (error < HIGH_THRESHOLD) {
      stats.correct_predictions_high++;
    }

    stats.total_samples++;
  }

  if (app_params_.mode == EMode::TrainTestMonitored) {
    updateMonitoredProgress(testResult);
  }
}

void TestingResult::updateMonitoredProgress(
    const TestingResult::TestResults &result) {
  if (app_params_.output_index_to_monitor > result.outputs.size()) {
    throw TestingException("invalid output_index_to_monitor parameter");
  }
  auto outputMonitored = result.outputs.at(app_params_.output_index_to_monitor);
  auto expected = result.expecteds.at(app_params_.output_index_to_monitor);

  if (!progress.contains(result.lineNumber)) {
    progress[result.lineNumber] = {
        .current = outputMonitored, .expected = expected, .hasPrevious = false};
  } else {
    auto &progress_update = progress[result.lineNumber];
    progress_update.previous = progress_update.current;
    progress_update.current = outputMonitored;
    progress_update.hasPrevious = true;
  }
}

void TestingResult::calcStats() {
  if (app_params_.mode == EMode::TrainTestMonitored) {
    calculateConvergences();
  }
  calculatePercentages();
}

void TestingResult::calculateConvergences() {
  for (const auto &[line, converg] : progress) {
    if (converg.hasPrevious) {
      if (converg.expected == 1 && converg.current > converg.previous) {
        stats.good_convergence_one++;
        stats.good_convergence++;
      } else if (converg.expected == 0 && converg.current < converg.previous) {
        stats.good_convergence_zero++;
        stats.good_convergence++;
      }
    }
  }
}

void TestingResult::calculatePercentages() {
  stats.accuracy_avg = static_cast<float>(stats.correct_predictions_avg) /
                       (float)stats.total_samples * 100.0f;
  stats.accuracy_low = static_cast<float>(stats.correct_predictions_low) /
                       (float)stats.total_samples * 100.0f;
  stats.accuracy_high = static_cast<float>(stats.correct_predictions_high) /
                        (float)stats.total_samples * 100.0f;
  if (app_params_.mode == EMode::TrainTestMonitored) {
    stats.convergence = static_cast<float>(stats.good_convergence) /
                        (float)stats.total_samples * 100.0f;
    stats.convergence_zero = static_cast<float>(stats.good_convergence_zero) /
                             (float)stats.total_expected_zero * 100.0f;
    stats.convergence_one = static_cast<float>(stats.good_convergence_one) /
                            (float)stats.total_expected_one * 100.0f;
  }
}