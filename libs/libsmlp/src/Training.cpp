#include "Training.h"
#include "../../csv-parser/include/csv_parser.h"
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>
#include <string>
#include <vector>

using namespace std::string_view_literals;

Training::Training(Network *network, const std::string &data_file_path,
                   Optimizer *optimizer)
    : network_(network), data_file_path_(data_file_path),
      optimizer_(optimizer){};

int Training::Train(int num_epochs, bool output_at_end, size_t from_line,
                    size_t to_line) {
  // Checking parameters
  if (from_line > to_line && to_line > 0) {
    std::cerr << "[ERROR] from_line is greater than to_line." << std::endl;
  }

  // Check the data file
  std::ifstream data_file(data_file_path_);
  if (!data_file.is_open()) {
    std::cerr << "Failed to open data file: " << data_file_path_ << std::endl;
    return EXIT_FAILURE;
  }

  // Parse each line
  Csv::Parser parser;
  std::vector<std::vector<Csv::CellReference>> cell_refs;
  std::string line;
  std::vector<float> input;
  std::vector<float> expected_output;
  int code = EXIT_SUCCESS;
  for (int epoch = 0; epoch < num_epochs; epoch++) {
    // Reset the file pointer to the beginning of the file for each epoch
    data_file.clear();
    data_file.seekg(0, std::ios::beg);
    size_t line_number = 0;
    while (std::getline(data_file, line) &&
           (line_number <= to_line || to_line == 0)) {
      line_number++;
      if (line_number < from_line) {
        continue;
      }
      // Parse input and output values from the line
      std::string_view data(line);

      try {
        parser.parseTo(data, cell_refs);
      } catch (Csv::ParseError &ex) {
        std::cerr << "[ERROR] CSV parse error at line " << line_number << ": "
                  << ex.what() << std::endl;
        code = EXIT_FAILURE;
        continue;
      }

      if (cell_refs.empty()) {
        continue;
      }

      if (cell_refs.size() !=
          network_->GetInputSize() + network_->GetOutputSize()) {
        std::cerr << "[ERROR] Invalid columns at line " << line_number
                  << ": found " << cell_refs.size() << " columns instead of "
                  << network_->GetInputSize() + network_->GetOutputSize()
                  << std::endl;
        code = EXIT_FAILURE;
        continue;
      }

      // Get values
      auto getValue = [](std::vector<Csv::CellReference> const &cells) {
        return (float)cells[0].getDouble().value();
      };
      if (output_at_end) {
        for (auto const &value :
             std::ranges::subrange(cell_refs.begin(),
                                   cell_refs.begin() +
                                       network_->GetInputSize()) |
                 std::views::transform(getValue)) {
          input.push_back(value);
        }
        for (auto const &value :
             std::ranges::subrange(cell_refs.begin() + network_->GetInputSize(),
                                   cell_refs.end()) |
                 std::views::transform(getValue)) {
          expected_output.push_back(value);
        }
      } else {
        for (auto const &value :
             std::ranges::subrange(cell_refs.begin(),
                                   cell_refs.begin() +
                                       network_->GetOutputSize()) |
                 std::views::transform(getValue)) {
          expected_output.push_back(value);
        }
        for (auto const &value :
             std::ranges::subrange(cell_refs.begin() +
                                       network_->GetOutputSize(),
                                   cell_refs.end()) |
                 std::views::transform(getValue)) {
          input.push_back(value);
        }
      }

      // Perform forward propagation and backward propagation for the
      // current example
      network_->Forward(input);

      // Store the parsed output values in the network's output layer
      network_->GetOutputLayer()->SetExpectedOutputValues(expected_output);

      // Compare the predicted output values with the sample output values and
      // compute the loss.
      network_->Backward();

      // Update weights and biases using the optimizer
      network_->UpdateWeightsAndBiases();

      // Cleaning
      input.clear();
      expected_output.clear();
    }
    if (line_number == 0) {
      std::cerr << "[ERROR] Empty file" << std::endl;
      code = EXIT_FAILURE;
      break;
    }
  }

  data_file.close();
  return code;
}
