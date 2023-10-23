#include "Training.h"
#include "Common.h"
#include <chrono>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_view_literals;

void Training::train(const Parameters &params) {
  if (params.from_line > params.to_line && params.to_line > 0) {
    throw std::invalid_argument("from_line is greater than to_line.");
  }
  fileParser_.OpenFile();
  network_->learningRate = params.learning_rate;

  for (size_t epoch = 0; epoch < params.num_epochs; epoch++) {
    std::cout << "[INFO] Training epoch " << epoch + 1 << "/"
              << params.num_epochs << "... " << std::endl;
    fileParser_.ResetPos();
    for (size_t i = params.from_line; i < params.to_line; i++) {
      RecordResult result = fileParser_.ProcessLine(params);
      if (result.isSuccess) {
        network_->forwardPropagation(result.record.first);
        network_->backwardPropagation(result.record.second);
        network_->updateWeights();
      }
    }
  }
  fileParser_.CloseFile();
}

void Training::trainAndTest(const Parameters &params) {
  if (params.from_line > params.to_line && params.to_line > 0) {
    throw std::invalid_argument("from_line is greater than to_line.");
  }
  fileParser_.OpenFile();
  Testing testing(network_, params.data_file);
  network_->learningRate = params.learning_rate;

  const auto start{std::chrono::steady_clock::now()};
  for (size_t epoch = 0; epoch < params.num_epochs; epoch++) {
    std::cout << "[INFO] Training epoch " << epoch + 1 << "/"
              << params.num_epochs << "... ";
    fileParser_.ResetPos();
    for (size_t i = params.from_line; i < params.to_line; i++) {
      RecordResult result = fileParser_.ProcessLine(params);
      if (result.isSuccess) {
        network_->forwardPropagation(result.record.first);
        network_->backwardPropagation(result.record.second);
        network_->updateWeights();
      }
    }

    std::cout << "testing... ";
    testing.test(params, epoch);
    testing.showResultsLine();
    std::cout << std::endl;
  }
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{end - start};
  std::cout << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
  testing.showResults(params.verbose);

  fileParser_.CloseFile();
}
