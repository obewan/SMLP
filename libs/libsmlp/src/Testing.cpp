#include "Testing.h"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>

using namespace std::string_view_literals;

bool Testing::Test(bool output_at_end, size_t from_line, size_t to_line) {
  if (from_line > to_line && to_line > 0) {
    std::cerr << "[ERROR] from_line is greater than to_line." << std::endl;
    return false;
  }

  if (!OpenFile()) {
    return false;
  }

  // Loop through the test data and evaluate the network's performance
  size_t input_size = network_->inputLayer.neurons.size();
  size_t ouput_size = network_->outputLayer.neurons.size();
  int correct_predictions = 0;
  int total_samples = 0;

  RecordFunction recordFunction =
      [&network = network_, &correct_predictions, &total_samples](
          std::pair<std::vector<float>, std::vector<float>> const &record) {
        auto predicteds = network->forwardPropagation(record.first);
        for (size_t i = 0; i < predicteds.size(); i++) {
          std::cout << "Expected:" << record.second[i]
                    << " Predicted:" << predicteds[i] << std::endl;
          if (predicteds[i] == record.second[i]) {
            correct_predictions++;
          }
        }
        total_samples++;
      };

  try {
    if (!ProcessFile(from_line, to_line, input_size, ouput_size, output_at_end,
                     recordFunction)) {
      std::cerr << "[ERROR] Error during file processing." << std::endl;
      return false;
    }
  } catch (std::exception &ex) {
    std::cerr << "[ERROR] Exception during file processing: " << ex.what()
              << std::endl;
    return false;
  }

  float accuracy = 0;
  if (total_samples > 0) {
    accuracy =
        static_cast<float>(correct_predictions) / (float)total_samples * 100.0f;
  }
  std::cout << "Accuracy: " << accuracy << "%" << std::endl;

  CloseFile();

  return true;
}