#include "TestingResult.h"
#include "../../json/include/json.hpp"
#include "CommonModes.h"
#include "Manager.h"
#include "exception/TestingException.h"
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>

const float LOW_THRESHOLD = 0.30f;
const float MEDIUM_THRESHOLD = 0.20f;
const float HIGH_THRESHOLD = 0.10f;

using namespace smlp;

std::string TestingResult::getResultsLine() {
  calcStats();
  std::stringstream sstr;
  const auto &app_params = Manager::getInstance().app_params;

  sstr << std::setprecision(2) << std::fixed << "acc(lah)["
       << stats.accuracy_low << " " << stats.accuracy_avg << " "
       << stats.accuracy_high << "]";
  if (app_params.mode == EMode::TrainTestMonitored) {
    sstr << std::setprecision(2) << std::fixed << " conv(01t)["
         << stats.convergence_zero << " " << stats.convergence_one << " "
         << stats.convergence << "]";
  }

  return sstr.str();
}

std::string TestingResult::getResultsJson() {
  calcStats();
  nlohmann::json json;
  json["accuracy_low"] = stats.accuracy_low;
  json["accuracy_avg"] = stats.accuracy_avg;
  json["accuracy_high"] = stats.accuracy_high;
  json["convergence_zero"] = stats.convergence_zero;
  json["convergence_one"] = stats.convergence_one;
  json["convergence"] = stats.convergence;
  return json.dump();
}

std::string TestingResult::getResultsDetailled() {
  if (!stats.isCalculate) {
    calcStats();
  }
  std::stringstream sstr;

  sstr << "Testing results: " << std::endl;

  sstr << showAccuracyResults();
  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.mode == EMode::TrainTestMonitored) {
    sstr << showConvergenceResults();
  }
  return sstr.str();
}

std::string TestingResult::showAccuracyResults() const {
  std::stringstream sstr;
  sstr << std::setprecision(3) << std::fixed
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
  sstr << std::setprecision(3) << std::fixed
       << "Convergence toward zero: " << stats.convergence_zero << "% ("
       << stats.good_convergence_zero << "/" << stats.total_expected_zero << ")"
       << std::endl
       << "Convergence toward one: " << stats.convergence_one << "%  ("
       << stats.good_convergence_one << "/" << stats.total_expected_one << ")"
       << std::endl
       << "Convergence total: " << stats.convergence << "% ("
       << stats.good_convergence << "/" << stats.total_convergences << ")"
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

  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.mode == EMode::TrainTestMonitored) {
    updateMonitoredProgress(testResult);
  }
}

void TestingResult::updateMonitoredProgress(
    const TestingResult::TestResults &result) {
  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.output_index_to_monitor > result.outputs.size()) {
    throw TestingException("invalid output_index_to_monitor parameter");
  }
  auto outputMonitored = result.outputs.at(app_params.output_index_to_monitor);
  auto expected = result.expecteds.at(app_params.output_index_to_monitor);

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
  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.mode == EMode::TrainTestMonitored) {
    calculateConvergences();
  }
  calculatePercentages();
  stats.isCalculate = true;
}

void TestingResult::calculateConvergences() {
  for (const auto &[line, converg] : progress) {
    if (converg.hasPrevious) {
      stats.total_convergences++;
      if (converg.expected == 1) {
        stats.total_expected_one++;
      } else {
        stats.total_expected_zero++;
      }
      if (converg.expected == 1 && (converg.current > converg.previous ||
                                    converg.current == converg.expected)) {
        stats.good_convergence_one++;
        stats.good_convergence++;
      } else if (converg.expected == 0 &&
                 (converg.current < converg.previous ||
                  converg.current == converg.expected)) {
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
  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.mode == EMode::TrainTestMonitored) {
    stats.convergence = static_cast<float>(stats.good_convergence) /
                        (float)stats.total_convergences * 100.0f;
    stats.convergence_zero = static_cast<float>(stats.good_convergence_zero) /
                             (float)stats.total_expected_zero * 100.0f;
    stats.convergence_one = static_cast<float>(stats.good_convergence_one) /
                            (float)stats.total_expected_one * 100.0f;
  }
}