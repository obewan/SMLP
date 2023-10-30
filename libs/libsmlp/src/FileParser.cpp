#include "FileParser.h"
#include "Common.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

FileParser::~FileParser() {
  if (file.is_open()) {
    file.close();
  }
}

void FileParser::openFile() {
  if (file.is_open()) {
    return;
  }
  file.open(path);
  if (!file.is_open()) {
    throw FileParserException("Failed to open file: " + path);
  }
  current_line_number = 0;
}

void FileParser::closeFile() {
  if (file.is_open()) {
    file.close();
  }
}

void FileParser::resetPos() {
  file.clear();
  file.seekg(0, std::ios::beg);
  current_line_number = 0;
}

RecordResult FileParser::processLine(const NetworkParameters &params,
                                     bool isTesting) {
  std::vector<std::vector<Csv::CellReference>> cell_refs;
  std::string line;
  current_line_number++;

  // if isTesting, skipping lines until testing lines
  if (isTesting && current_line_number < training_ratio_line) {
    for (; current_line_number < training_ratio_line; ++current_line_number) {
      if (!std::getline(file, line)) {
        return {.isSuccess = false, .isEndOfFile = true};
      }
    }
  }

  if (!std::getline(file, line)) {
    return {.isSuccess = false, .isEndOfFile = true};
  }

  try {
    std::string_view data(line);
    csv_parser.parseTo(data, cell_refs);
  } catch (Csv::ParseError &ex) {
    std::stringstream sstr;
    sstr << "CSV parsing error at line " << current_line_number << ": "
         << ex.what();
    throw FileParserException(sstr.str());
  }

  if (cell_refs.empty() ||
      cell_refs.size() != params.input_size + params.output_size) {
    std::stringstream sstr;
    sstr << "Invalid columns at line " << current_line_number << ": found "
         << cell_refs.size() << " columns instead of "
         << params.input_size + params.output_size;
    throw FileParserException(sstr.str());
  }

  Record record = params.output_at_end
                      ? processInputFirst(cell_refs, params.input_size)
                      : processOutputFirst(cell_refs, params.output_size);
  return {.isSuccess = true, .record = record};
}

Record FileParser::processInputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size) const {
  std::vector<float> input;
  std::vector<float> expected_output;
  auto getValue = [](auto cells) {
    return (float)cells[0].getDouble().value();
  };

  for (auto const &value :
       std::ranges::subrange(cell_refs.begin(),
                             cell_refs.begin() + input_size) |
           std::views::transform(getValue)) {
    input.push_back(value);
  }
  for (auto const &value :
       std::ranges::subrange(cell_refs.begin() + input_size, cell_refs.end()) |
           std::views::transform(getValue)) {
    expected_output.push_back(value);
  }
  return std::make_pair(input, expected_output);
}

Record FileParser::processOutputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t output_size) const {
  std::vector<float> input;
  std::vector<float> expected_output;
  auto getValue = [](auto cells) {
    return (float)cells[0].getDouble().value();
  };

  for (auto const &value :
       std::ranges::subrange(cell_refs.begin(),
                             cell_refs.begin() + output_size) |
           std::views::transform(getValue)) {
    expected_output.push_back(value);
  }
  for (auto const &value :
       std::ranges::subrange(cell_refs.begin() + output_size, cell_refs.end()) |
           std::views::transform(getValue)) {
    input.push_back(value);
  }
  return std::make_pair(input, expected_output);
}