#include "DataFileParser.h"
#include "Common.h"
#include "exception/FileParserException.h"
#include <iostream>
#include <sstream>
#include <string>

DataFileParser::~DataFileParser() {
  if (file.is_open()) {
    file.close();
  }
}

void DataFileParser::openFile(const std::string &filepath) {
  if (file.is_open()) {
    if (filepath.empty()) {
      return;
    } else {
      file.close();
    }
  }
  if (!filepath.empty()) {
    path = filepath;
  }
  file.open(path);
  if (!file.is_open()) {
    throw FileParserException("Failed to open file: " + path);
  }
  current_line_number = 0;
}

void DataFileParser::closeFile() {
  if (file.is_open()) {
    file.close();
  }
}

void DataFileParser::resetPos() {
  file.clear();
  file.seekg(0, std::ios::beg);
  current_line_number = 0;
}

RecordResult
DataFileParser::processLine(const NetworkParameters &network_params,
                            const AppParameters &app_params,
                            const std::string &line) {

  std::string nextline;
  current_line_number++;
  if (line.empty()) {
    if (!getNextLine(nextline, app_params)) {
      return {.isSuccess = false, .isEndOfFile = true};
    }
  } else {
    nextline = line;
  }

  std::vector<std::vector<Csv::CellReference>> cell_refs;

  parseLine(nextline, cell_refs);

  validateColumns(cell_refs, network_params, app_params);

  Record record = processColumns(cell_refs, network_params, app_params);

  return {.isSuccess = true, .record = record};
}

bool DataFileParser::getNextLine(std::string &line,
                                 const AppParameters &app_params) {
  // if isTesting, skipping lines until testing lines
  if (app_params.use_training_ratio_line &&
      current_line_number < training_ratio_line) {
    for (; current_line_number < training_ratio_line; ++current_line_number) {
      if (!std::getline(file, line)) {
        return false;
      }
    }
  }
  if (!std::getline(file, line)) {
    return false;
  }
  return true;
}

void DataFileParser::parseLine(
    const std::string &line,
    std::vector<std::vector<Csv::CellReference>> &cell_refs) const {
  try {
    std::string_view data(line);
    csv_parser.parseTo(data, cell_refs);
  } catch (Csv::ParseError &ex) {
    std::stringstream sstr;
    sstr << "CSV parsing error at line " << current_line_number << ": "
         << ex.what();
    throw FileParserException(sstr.str());
  }
}

void DataFileParser::validateColumns(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    const NetworkParameters &network_params,
    const AppParameters &app_params) const {
  if (cell_refs.empty()) {
    std::stringstream sstr;
    sstr << "Invalid columns at line " << current_line_number << ": empty line";
    throw FileParserException(sstr.str());
  }

  if (app_params.mode != EMode::Predictive &&
      cell_refs.size() !=
          network_params.input_size + network_params.output_size) {
    std::stringstream sstr;
    sstr << "Invalid columns at line " << current_line_number << ": found "
         << cell_refs.size() << " columns instead of "
         << network_params.input_size + network_params.output_size;
    throw FileParserException(sstr.str());
  }

  if (app_params.mode == EMode::Predictive &&
      cell_refs.size() < network_params.input_size) {
    std::stringstream sstr;
    sstr << "Invalid columns at line " << current_line_number << ": found "
         << cell_refs.size() << " columns instead of a minimum of "
         << network_params.input_size;
    throw FileParserException(sstr.str());
  }
}

Record DataFileParser::processColumns(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    const NetworkParameters &network_params,
    const AppParameters &app_params) const {
  Record record;
  try {
    if (app_params.mode == EMode::Predictive &&
        cell_refs.size() == network_params.input_size) {
      record = processInputOnly(cell_refs, network_params.input_size);
    } else if (app_params.output_at_end) {
      record = processInputFirst(cell_refs, network_params.input_size,
                                 network_params.output_size);
    } else {
      record = processOutputFirst(cell_refs, network_params.input_size,
                                  network_params.output_size);
    }
  } catch (std::bad_optional_access &) {
    std::stringstream sstr;
    sstr << "CSV parsing error at line " << current_line_number
         << ": bad column format";
    throw FileParserException(sstr.str());
  }
  return record;
}

Record DataFileParser::processInputOnly(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size) const {
  std::vector<float> input(input_size);
  auto getValue = [](auto cells) {
    return (float)cells[0].getDouble().value();
  };
  std::transform(cell_refs.begin(), cell_refs.begin() + input_size,
                 input.begin(), getValue);
  return std::make_pair(input, std::vector<float>{});
}

Record DataFileParser::processInputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size, size_t output_size) const {
  std::vector<float> input(input_size);
  std::vector<float> expected_output(output_size);
  auto getValue = [](auto cells) {
    return (float)cells[0].getDouble().value();
  };
  std::transform(cell_refs.begin(), cell_refs.begin() + input_size,
                 input.begin(), getValue);
  std::transform(cell_refs.begin() + input_size, cell_refs.end(),
                 expected_output.begin(), getValue);
  return std::make_pair(input, expected_output);
}

Record DataFileParser::processOutputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size, size_t output_size) const {
  std::vector<float> input(input_size);
  std::vector<float> expected_output(output_size);
  auto getValue = [](auto cells) {
    return (float)cells[0].getDouble().value();
  };
  std::transform(cell_refs.begin(), cell_refs.begin() + output_size,
                 expected_output.begin(), getValue);
  std::transform(cell_refs.begin() + output_size, cell_refs.end(),
                 input.begin(), getValue);
  return std::make_pair(input, expected_output);
}