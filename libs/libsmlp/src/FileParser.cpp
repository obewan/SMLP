#include "FileParser.h"
#include "Common.h"
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <stdexcept>

FileParser::~FileParser() {
  if (file_.is_open()) {
    file_.close();
  }
}

bool FileParser::OpenFile() {
  file_.open(path_);
  if (!file_.is_open()) {
    std::cerr << "Failed to open file: " << path_ << std::endl;
    return false;
  }
  line_number = 0;
  return true;
}

void FileParser::CloseFile() {
  if (file_.is_open()) {
    file_.close();
  }
}

void FileParser::ResetPos() {
  file_.clear();
  file_.seekg(0, std::ios::beg);
  line_number = 0;
}

bool FileParser::ProcessFile(size_t epoch, size_t from_line, size_t to_line,
                             size_t input_size, size_t output_size,
                             bool output_at_end,
                             const RecordFunction &processRecord) {
  size_t line_number = 0;
  std::string line;
  while (std::getline(file_, line) &&
         (line_number <= to_line || to_line == 0)) {
    line_number++;
    if (line_number < from_line) {
      continue;
    }
    if (!ProcessLine(epoch, line, line_number, input_size, output_size,
                     output_at_end, processRecord)) {
      return false;
    }
  }

  if (line_number == 0) {
    std::cerr << "[ERROR] Empty file" << std::endl;
    return false;
  }

  return true;
}

bool FileParser::ProcessLine(size_t epoch, const std::string &line,
                             size_t line_number, size_t input_size,
                             size_t output_size, bool output_at_end,
                             const RecordFunction &processRecord) const {
  std::vector<std::vector<Csv::CellReference>> cell_refs;
  size_t total_columns = input_size + output_size;
  std::string_view data(line);

  try {
    parser_.parseTo(data, cell_refs);
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

  std::pair<std::vector<float>, std::vector<float>>
      record; // pair of <inputs, outputs>
  if (output_at_end) {
    record = ProcessInputFirst(cell_refs, input_size);
  } else {
    record = ProcessOutputFirst(cell_refs, output_size);
  }

  processRecord(epoch, line_number, record);

  return true;
}

RecordResult FileParser::ProcessLine(const Parameters &params) {
  std::vector<std::vector<Csv::CellReference>> cell_refs;
  std::string line;
  line_number++;

  // Skipping lines until from_line
  for (; line_number < params.from_line; ++line_number) {
    if (!std::getline(file_, line)) {
      return {.isSuccess = false, .isEndOfFile = true};
    }
  }

  if (!std::getline(file_, line)) {
    return {.isSuccess = false, .isEndOfFile = true};
  }

  try {
    std::string_view data(line);
    parser_.parseTo(data, cell_refs);
  } catch (Csv::ParseError &ex) {
    std::stringstream sstr;
    sstr << "CSV parsing error at line " << line_number << ": " << ex.what();
    throw std::runtime_error(sstr.str());
  }

  if (cell_refs.empty() ||
      cell_refs.size() != params.input_size + params.output_size) {
    std::stringstream sstr;
    sstr << "Invalid columns at line " << line_number << ": found "
         << cell_refs.size() << " columns instead of "
         << params.input_size + params.output_size;
    throw std::runtime_error(sstr.str());
  }

  Record record = params.output_at_end
                      ? ProcessInputFirst(cell_refs, params.input_size)
                      : ProcessOutputFirst(cell_refs, params.output_size);
  return {.isSuccess = true, .record = record};
}

Record FileParser::ProcessInputFirst(
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

Record FileParser::ProcessOutputFirst(
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