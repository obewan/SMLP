#include "Training.h"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_view_literals;

bool Training::Train(size_t num_epochs, float learning_rate, bool output_at_end,
                     size_t from_line, size_t to_line) {
  if (from_line > to_line && to_line > 0) {
    std::cerr << "[ERROR] from_line is greater than to_line." << std::endl;
    return false;
  }

  if (!OpenFile()) {
    return false;
  }

  bool isSuccess = true;
  for (size_t epoch = 0; epoch < num_epochs; epoch++) {
    std::cout << "[INFO] Training epoch " << epoch + 1 << "/" << num_epochs
              << std::endl;
    if (!ProcessEpoch(epoch, learning_rate, from_line, to_line,
                      output_at_end)) {
      isSuccess = false;
      break;
    }
  }

  CloseFile();

  return isSuccess;
}

bool Training::TrainAndTest(Testing &testing, size_t num_epochs,
                            float learning_rate, bool output_at_end,
                            size_t from_line, size_t to_line) {
  if (from_line > to_line && to_line > 0) {
    std::cerr << "[ERROR] from_line is greater than to_line." << std::endl;
    return false;
  }

  if (!OpenFile()) {
    return false;
  }

  bool isSuccess = true;
  for (size_t epoch = 0; epoch < num_epochs; epoch++) {
    std::cout << "[INFO] Training epoch " << epoch + 1 << "/" << num_epochs
              << std::endl;
    if (!ProcessEpoch(epoch, learning_rate, from_line, to_line,
                      output_at_end)) {
      isSuccess = false;
      break;
    }
    testing.Test(output_at_end, from_line, to_line, epoch);
  }

  CloseFile();

  return isSuccess;
}

bool Training::ProcessEpoch(size_t epoch, float learning_rate, size_t from_line,
                            size_t to_line, bool output_at_end) {
  ResetPos();
  size_t input_size = network_->inputLayer.neurons.size();
  size_t ouput_size = network_->outputLayer.neurons.size();
  network_->learningRate = learning_rate;
  RecordFunction recordFunction =
      [&network = network_](
          size_t epoch, size_t line_number,
          std::pair<std::vector<float>, std::vector<float>> const &record) {
        network->forwardPropagation(record.first);
        network->backwardPropagation(record.second);
        network->updateWeights();
      };

  try {
    if (!ProcessFile(epoch, from_line, to_line, input_size, ouput_size,
                     output_at_end, recordFunction)) {
      std::cerr << "[ERROR] Error during file processing." << std::endl;
      return false;
    }
  } catch (std::exception &ex) {
    std::cerr << "[ERROR] Exception during file processing: " << ex.what()
              << std::endl;
    return false;
  }

  return true;
}
