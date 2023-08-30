#include "Training.h"
#include <cstddef>
#include <cstdlib>
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

bool Training::Train(size_t num_epochs, bool output_at_end, size_t from_line,
                     size_t to_line) {
  if (from_line > to_line && to_line > 0) {
    std::cerr << "[ERROR] from_line is greater than to_line." << std::endl;
    return false;
  }

  if (!OpenDataFile()) {
    return false;
  }

  Csv::Parser parser;
  bool isSuccess = true;
  for (size_t epoch = 0; epoch < num_epochs; epoch++) {
    if (!ProcessEpoch(parser, from_line, to_line, output_at_end)) {
      isSuccess = false;
      break;
    }
  }

  data_file_.close();
  return isSuccess;
}

bool Training::OpenDataFile() {
  data_file_.open(data_file_path_);
  if (!data_file_.is_open()) {
    std::cerr << "Failed to open data file: " << data_file_path_ << std::endl;
    return false;
  }
  return true;
}

bool Training::ProcessEpoch(const Csv::Parser &parser, size_t from_line,
                            size_t to_line, bool output_at_end) {
  data_file_.clear();
  data_file_.seekg(0, std::ios::beg);
  size_t line_number = 0;
  std::string line;
  while (std::getline(data_file_, line) &&
         (line_number <= to_line || to_line == 0)) {
    line_number++;
    if (line_number < from_line) {
      continue;
    }
    if (!ProcessLine(parser, line, line_number, output_at_end)) {
      return false;
    }
  }

  if (line_number == 0) {
    std::cerr << "[ERROR] Empty file" << std::endl;
    return false;
  }

  return true;
}

bool Training::ProcessLine(const Csv::Parser &parser, const std::string &line,
                           size_t line_number, bool output_at_end) {
  std::vector<float> input;
  std::vector<float> expected_output;
  std::vector<std::vector<Csv::CellReference>> cell_refs;
  size_t total_columns = network_->GetInputSize() + network_->GetOutputSize();
  std::string_view data(line);

  try {
    parser.parseTo(data, cell_refs);
  } catch (Csv::ParseError &ex) {
    std::cerr << "[ERROR] CSV parse error at line " << line_number << ": "
              << ex.what() << std::endl;
    return false;
  }

  if (cell_refs.empty() || cell_refs.size() != total_columns) {
    std::cerr << "[ERROR] Invalid columns at line " << line_number << ": found "
              << cell_refs.size() << " columns instead of " << total_columns
              << std::endl;
    return false;
  }

  auto getValue = [](std::vector<Csv::CellReference> const &cells) {
    return (float)cells[0].getDouble().value();
  };

  if (output_at_end) {
    ProcessInputFirst(cell_refs, input, expected_output, getValue);
  } else {
    ProcessOutputFirst(cell_refs, input, expected_output, getValue);
  }

  network_->Forward(input);
  network_->GetOutputLayer()->SetExpectedOutputValues(expected_output);
  network_->Backward();
  network_->UpdateWeightsAndBiases();

  input.clear();
  expected_output.clear();

  return true;
}

void Training::ProcessInputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    std::vector<float> &input, std::vector<float> &expected_output,
    const std::function<float(std::vector<Csv::CellReference> const &)>
        &getValue) const {
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
}

void Training::ProcessOutputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    std::vector<float> &input, std::vector<float> &expected_output,
    const std::function<float(std::vector<Csv::CellReference> const &)>
        &getValue) const {
  for (auto const &value :
       std::ranges::subrange(cell_refs.begin(),
                             cell_refs.begin() + network_->GetOutputSize()) |
           std::views::transform(getValue)) {
    expected_output.push_back(value);
  }
  for (auto const &value :
       std::ranges::subrange(cell_refs.begin() + network_->GetOutputSize(),
                             cell_refs.end()) |
           std::views::transform(getValue)) {
    input.push_back(value);
  }
}
