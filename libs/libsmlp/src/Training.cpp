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

int Training::Train(int num_epochs) {
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
  std::vector<float> output;
  int code = EXIT_SUCCESS;
  for (int epoch = 0; epoch < num_epochs; epoch++) {
    // Reset the file pointer to the beginning of the file for each epoch
    data_file.clear();
    data_file.seekg(0, std::ios::beg);
    int line_number = 0;
    while (std::getline(data_file, line)) {
      // Parse input and output values from the line
      std::string_view data = line;
      line_number++;
      try {
        parser.parseTo(data, cell_refs);
      } catch (Csv::ParseError &ex) {
        std::cerr << "CSV parse error at line " << line_number << ": "
                  << ex.what() << std::endl;
        code = EXIT_FAILURE;
        continue;
      }
      if (cell_refs.empty()) {
        continue;
      }
      if (cell_refs[0].size() !=
          network_->GetInputSize() + network_->GetOutputSize()) {
        std::cerr << "Invalid columns at line " << line_number << ": found "
                  << cell_refs[0].size() << " columns instead of "
                  << network_->GetInputSize() + network_->GetOutputSize()
                  << std::endl;
        code = EXIT_FAILURE;
        continue;
      }

      // Get values
      auto getValue = [](Csv::CellReference const &cell) {
        return (float)cell.getDouble().value();
      };
      for (auto const &value :
           std::ranges::subrange(cell_refs[0].begin(),
                                 cell_refs[0].begin() +
                                     network_->GetInputSize()) |
               std::views::transform(getValue)) {
        input.push_back(value);
      }
      for (auto const &value :
           std::ranges::subrange(cell_refs[0].begin() +
                                     network_->GetInputSize() + 1,
                                 cell_refs[0].end()) |
               std::views::transform(getValue)) {
        output.push_back(value);
      }

      // Perform forward propagation and backward propagation for the
      // current example
      network_->Forward(input);

      // Store the parsed output values in the network's output layer
      network_->GetOutputLayer()->SetOutputTargetValues(output);

      // Compare the predicted output values with the sample output values and
      // compute the loss.
      network_->Backward();

      // Update weights and biases using the optimizer
      network_->UpdateWeightsAndBiases();

      // Cleaning
      input.clear();
      output.clear();
    }
    if (line_number == 0) {
      std::cerr << "Empty file" << std::endl;
      code = EXIT_FAILURE;
      break;
    }
  }

  data_file.close();
  return code;
}
