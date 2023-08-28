#include "Testing.h"
#include "../../csv-parser/include/csv_parser.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>

using namespace std::string_view_literals;

int Testing::Test() {
  // Load test data from the file
  std::ifstream test_data_file(test_data_file_path_);
  if (!test_data_file.is_open()) {
    std::cerr << "[ERROR] Failed to open test data file: "
              << test_data_file_path_ << std::endl;
    return EXIT_FAILURE;
  }

  // Loop through the test data and evaluate the network's performance
  int correct_predictions = 0;
  int total_samples = 0;

  Csv::Parser parser;
  std::vector<std::vector<Csv::CellReference>> cell_refs;
  std::string line;
  std::vector<float> input;
  std::vector<float> expected_output;
  int code = EXIT_SUCCESS;
  int line_number = 0;
  while (std::getline(test_data_file, line)) {
    // Parse input and output values from the line
    std::string_view data(line);
    line_number++;

    // Parse input and expected output values from the line
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
    for (auto const &value :
         std::ranges::subrange(cell_refs.begin(),
                               cell_refs.begin() + network_->GetInputSize()) |
             std::views::transform(getValue)) {
      input.push_back(value);
    }
    for (auto const &value :
         std::ranges::subrange(cell_refs.begin() + network_->GetInputSize(),
                               cell_refs.end()) |
             std::views::transform(getValue)) {
      expected_output.push_back(value);
    }

    // Forward pass through the network to get predicted output
    std::vector<float> predicted_output = network_->Predict(input);

    // Compare predicted_output with expected_output and calculate metrics
    for (size_t i = 0; i < predicted_output.size(); i++) {
      std::cout << "Expected:" << expected_output[i]
                << " predicted:" << predicted_output[i] << std::endl;
      if (predicted_output[i] == expected_output[i]) {
        correct_predictions++;
      }
    }

    total_samples++;

    // Cleaning
    input.clear();
    expected_output.clear();
  }

  if (line_number == 0) {
    std::cerr << "[ERROR] Empty file" << std::endl;
    code = EXIT_FAILURE;
  }

  test_data_file.close();

  // Calculate and display metrics (e.g., accuracy)
  float accuracy =
      static_cast<float>(correct_predictions) / (float)total_samples * 100.0f;
  std::cout << "Accuracy: " << accuracy << "%" << std::endl;
  return code;
}