#include "Testing.h"
#include "Common.h"
#include "exception/TestingException.h"
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>

using namespace std::string_view_literals;

void Testing::test(const NetworkParameters &network_params,
                   const AppParameters &app_params, size_t epoch) {
  if (app_params.use_testing_ratio_line) {
    if (!fileParser_->isTrainingRatioLineProcessed) {
      fileParser_->getTrainingRatioLine(app_params.training_ratio,
                                        app_params.training_ratio_line,
                                        app_params.use_stdin);
    }
    if (fileParser_->training_ratio_line >= fileParser_->total_lines) {
      throw TestingException("No data left for testing, check your "
                             "training_ratio parameter. Aborting testing.");
    }
  }

  if (!fileParser_->file.is_open()) {
    fileParser_->openFile();
  }

  std::vector<TestResults> testResults;
  bool isTesting = true;
  int output_neuron_to_monitor = (int)app_params.output_index_to_monitor - 1;
  while (isTesting) {
    RecordResult result = fileParser_->processLine(network_params, app_params);
    if (result.isSuccess) {
      auto predicteds = network_->forwardPropagation(result.record.first);
      // Using just one output neuron to monitor, or else there will be too much
      // memory used.
      if (output_neuron_to_monitor >= 0 &&
          output_neuron_to_monitor < (int)network_params.output_size) {
        testResults.emplace_back(epoch, fileParser_->current_line_number,
                                 result.record.second[output_neuron_to_monitor],
                                 predicteds[output_neuron_to_monitor]);
      }
    } else {
      isTesting = false;
    }
  }

  processResults(testResults, epoch);
}

void Testing::testLine(const NetworkParameters &network_params,
                       const AppParameters &app_params,
                       const RecordResult &record_result,
                       const size_t line_number,
                       std::vector<Testing::TestResults> &testResults) const {
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

Testing::Stat Testing::calcStats() {
  testResultExts.clear();
  for (auto const &result : lastEpochTestResultTemp_) {
    testResultExts.emplace_back(result.epoch, result.line, result.expected,
                                result.output, progress.at(result.line));
  }
  Testing::Stat stats;
  stats.total_samples = testResultExts.size();
  for (auto const &result : testResultExts) {
    if (result.expected == 1) {
      stats.total_expected_one++;
    } else {
      stats.total_expected_zero++;
    }

    if (result.progress.size() > 1) {
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
    if (abs(result.expected - result.output) < 0.30) {
      stats.correct_predictions_low++;
    }
    if (abs(result.expected - result.output) < 0.20) {
      stats.correct_predictions++;
    }
    if (abs(result.expected - result.output) < 0.10) {
      stats.correct_predictions_high++;
    }
  }

  if (stats.total_samples > 0) {
    stats.accuracy = static_cast<float>(stats.correct_predictions) /
                     (float)stats.total_samples * 100.0f;
    stats.accuracy_low = static_cast<float>(stats.correct_predictions_low) /
                         (float)stats.total_samples * 100.0f;
    stats.accuracy_high = static_cast<float>(stats.correct_predictions_high) /
                          (float)stats.total_samples * 100.0f;
    stats.convergence = static_cast<float>(stats.good_convergence) /
                        (float)stats.total_samples * 100.0f;
    stats.convergence_zero = static_cast<float>(stats.good_convergence_zero) /
                             (float)stats.total_expected_zero * 100.0f;
    stats.convergence_one = static_cast<float>(stats.good_convergence_one) /
                            (float)stats.total_expected_one * 100.0f;
  }
  return stats;
}

std::string Testing::showResultsLine(bool withConvergence) {
  auto stats = calcStats();
  std::stringstream sstr;

  sstr << std::setprecision(2) << "acc(lah)[" << stats.accuracy_low << " "
       << stats.accuracy << " " << stats.accuracy_high << "]";
  if (withConvergence) {
    sstr << std::setprecision(2) << " conv(01t)[" << stats.convergence_zero
         << " " << stats.convergence_one << " " << stats.convergence << "]";
  }

  return sstr.str();
}

std::string Testing::showResultsVerbose(const TestResultsExt &result,
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

std::string Testing::showResults(EMode mode, bool verbose) {
  auto stats = calcStats();
  std::stringstream sstr;

  sstr << "Testing results: " << std::endl;

  if (verbose) {
    for (auto const &result : testResultExts) {
      sstr << showResultsVerbose(result, mode) << std::endl;
    }
    sstr << std::endl;
  }

  sstr << std::setprecision(3)
       << "Low accuracy (correct at 70%): " << stats.accuracy_low << "%"
       << std::endl;
  sstr << std::setprecision(3)
       << "Avg accuracy (correct at 80%): " << stats.accuracy << "%"
       << std::endl;
  sstr << std::setprecision(3)
       << "High accuracy (correct at 90%): " << stats.accuracy_high << "%"
       << std::endl;

  if (mode == EMode::TrainTestMonitored) {
    sstr << std::setprecision(3)
         << "Good convergence toward zero: " << stats.convergence_zero << "% ("
         << stats.good_convergence_zero << "/" << stats.total_expected_zero
         << ")" << std::endl;
    sstr << std::setprecision(3)
         << "Good convergence toward one: " << stats.convergence_one << "%  ("
         << stats.good_convergence_one << "/" << stats.total_expected_one << ")"
         << std::endl;
    sstr << std::setprecision(3)
         << "Good convergence total: " << stats.convergence << "% ("
         << stats.good_convergence << "/" << stats.total_samples << ")"
         << std::endl;
  }
  return sstr.str();
}