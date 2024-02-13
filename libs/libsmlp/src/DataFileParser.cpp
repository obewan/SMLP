#include "DataFileParser.h"
#include "Common.h"
#include "CommonErrors.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "exception/FileParserException.h"
#include <string>

using namespace smlp;

DataFileParser::~DataFileParser() {
  if (file && file.is_open()) {
    file.close();
  }
}

void DataFileParser::openFile() {
  const auto &manager = Manager::getInstance();
  const auto &data_file = manager.app_params.data_file;
  if (file.is_open()) {
    if (data_file.empty()) {
      return;
    } else {
      file.close();
    }
  }
  if (data_file.empty()) {
    throw FileParserException(SimpleLang::Error(Error::FailedToOpenFile));
  }
  file.open(data_file);
  if (!file.is_open()) {
    throw FileParserException(SimpleLang::Error(Error::FailedToOpenFile) +
                              ": " + data_file);
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

RecordResult DataFileParser::processLine(const std::string &line) {
  std::string nextline;
  current_line_number++;
  if (line.empty()) {
    if (!getNextLine(nextline)) {
      return {.isSuccess = false, .isEndOfFile = true};
    }
  } else {
    nextline = line;
  }

  std::vector<std::vector<Csv::CellReference>> cell_refs;

  parseLine(nextline, cell_refs);

  validateColumns(cell_refs);

  Record record = processColumns(cell_refs);

  return {.isSuccess = true, .record = record};
}

bool DataFileParser::getNextLine(std::string &line) {
  // if isTesting, skipping lines until testing lines
  const auto &manager = Manager::getInstance();
  if (manager.app_params.use_training_ratio_line &&
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
    throw FileParserException(
        SimpleLang::Error(
            Error::CSVParsingError,
            {{"line_number", std::to_string(current_line_number)}}) +
        ": " + ex.what());
  }
}

void DataFileParser::validateColumns(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs) const {
  if (cell_refs.empty()) {
    throw FileParserException(SimpleLang::Error(
        Error::CSVParsingErrorEmptyLine,
        {{"line_number", std::to_string(current_line_number)}}));
  }

  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  const auto &network_params = manager.network_params;

  if (app_params.mode != EMode::Predictive &&
      cell_refs.size() !=
          network_params.input_size + network_params.output_size) {
    throw FileParserException(SimpleLang::Error(
        Error::CSVParsingErrorColumnsSize,
        {{"line_number", std::to_string(current_line_number)},
         {"value", std::to_string(cell_refs.size())},
         {"expected", std::to_string(network_params.input_size +
                                     network_params.output_size)}}));
  }

  if (app_params.mode == EMode::Predictive &&
      cell_refs.size() < network_params.input_size) {
    throw FileParserException(SimpleLang::Error(
        Error::CSVParsingErrorColumnsMin,
        {{"line_number", std::to_string(current_line_number)},
         {"value", std::to_string(cell_refs.size())},
         {"expected", std::to_string(network_params.input_size)}}));
  }
}

Record DataFileParser::processColumns(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs) const {
  Record record;
  try {
    const auto &manager = Manager::getInstance();
    const auto &app_params = manager.app_params;
    const auto &network_params = manager.network_params;

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
    throw FileParserException(SimpleLang::Error(
        Error::CSVParsingErrorColumnsBadFormat,
        {{"line_number", std::to_string(current_line_number)}}));
  }
  return record;
}

Record DataFileParser::processInputOnly(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size) const {
  std::vector<float> inputs(input_size);

  std::transform(cell_refs.begin(), cell_refs.begin() + input_size,
                 inputs.begin(), getFloatValue);
  return {.inputs = inputs, .outputs = std::vector<float>{}};
}

Record DataFileParser::processInputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size, size_t output_size) const {
  std::vector<float> inputs(input_size);
  std::vector<float> expected_outputs(output_size);

  std::transform(cell_refs.begin(), cell_refs.begin() + input_size,
                 inputs.begin(), getFloatValue);
  std::transform(cell_refs.begin() + input_size, cell_refs.end(),
                 expected_outputs.begin(), getFloatValue);
  return {.inputs = inputs, .outputs = expected_outputs};
}

Record DataFileParser::processOutputFirst(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size, size_t output_size) const {
  std::vector<float> inputs(input_size);
  std::vector<float> expected_outputs(output_size);

  std::transform(cell_refs.begin(), cell_refs.begin() + output_size,
                 expected_outputs.begin(), getFloatValue);
  std::transform(cell_refs.begin() + output_size, cell_refs.end(),
                 inputs.begin(), getFloatValue);
  return {.inputs = inputs, .outputs = expected_outputs};
}