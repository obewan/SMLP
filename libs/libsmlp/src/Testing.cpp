#include "Testing.h"
#include "Common.h"
#include <iomanip>
#include <iostream>
#include <ranges>

using namespace std::string_view_literals;

void Testing::test(const Parameters &params, size_t epoch) {
  if (!fileParser_->isTrainingRatioLineProcessed) {
    fileParser_->getTrainingRatioLine(params.training_ratio);
  }
  if (fileParser_->training_ratio_line >= fileParser_->total_lines) {
    std::cout
        << "Warning: No lines left for testing, check your training_ratio "
           "parameter. Aborting testing"
        << std::endl;
    return;
  }

  if (!fileParser_->file.is_open()) {
    fileParser_->openFile();
  }

  std::vector<TestResults> testResults;
  bool isTesting = true;
  int output_neuron_to_monitor = (int)params.output_index_to_monitor - 1;
  while (isTesting) {
    RecordResult result = fileParser_->processLine(params, true);
    if (result.isSuccess) {
      auto predicteds = network_->forwardPropagation(result.record.first);
      // Using just one output neuron to monitor, or else there will be too much
      // memory used.
      if (output_neuron_to_monitor >= 0 &&
          output_neuron_to_monitor < (int)params.output_size) {
        testResults.emplace_back(epoch, fileParser_->current_line_number,
                                 result.record.second[output_neuron_to_monitor],
                                 predicteds[output_neuron_to_monitor]);
      }
    } else {
      isTesting = false;
    }
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

void Testing::showResultsLine() {
  auto stats = calcStats();
  const auto default_precision{std::cout.precision()};

  std::cout << std::setprecision(2) << "acc(lah)[" << stats.accuracy_low << " "
            << stats.accuracy << " " << stats.accuracy_high << "] ";
  std::cout << std::setprecision(2) << "conv(01t)[" << stats.convergence_zero
            << " " << stats.convergence_one << " " << stats.convergence << "]";

  std::cout << std::setprecision((int)default_precision); // restore defaults
}

void Testing::showResults(bool verbose) {
  auto stats = calcStats();

  if (verbose) {
    for (auto const &result : testResultExts) {
      std::cout << "Expected:" << result.expected
                << " Predicted:" << result.output << " [ ";
      for (auto &progres : result.progress) {
        std::cout << progres << " ";
      }
      std::cout << "] (" << result.progress.back() - result.progress.front()
                << ")" << std::endl;
    }
    std::cout << std::endl;
  }

  const auto default_precision{std::cout.precision()};
  std::cout << std::setprecision(3)
            << "Low accuracy (correct at 70%): " << stats.accuracy_low << "%"
            << std::endl;
  std::cout << std::setprecision(3)
            << "Avg accuracy (correct at 80%): " << stats.accuracy << "%"
            << std::endl;
  std::cout << std::setprecision(3)
            << "High accuracy (correct at 90%): " << stats.accuracy_high << "%"
            << std::endl;

  if (network_->params.mode == Mode::TrainTestMonitored) {
    std::cout << std::setprecision(3)
              << "Good convergence toward zero: " << stats.convergence_zero
              << "% (" << stats.good_convergence_zero << "/"
              << stats.total_expected_zero << ")" << std::endl;
    std::cout << std::setprecision(3)
              << "Good convergence toward one: " << stats.convergence_one
              << "%  (" << stats.good_convergence_one << "/"
              << stats.total_expected_one << ")" << std::endl;
    std::cout << std::setprecision(3)
              << "Good convergence total: " << stats.convergence << "% ("
              << stats.good_convergence << "/" << stats.total_samples << ")"
              << std::endl;
  }
  std::cout << std::setprecision((int)default_precision); // restore defaults
}