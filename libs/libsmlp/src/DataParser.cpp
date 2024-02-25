#include "DataParser.h"
#include "Common.h"
#include "Manager.h"
#include "SimpleLang.h"

using namespace smlp;

RecordResult DataParser::processLine(const std::string &line, bool isTesting) {
  if (line.empty()) {
    return {.isSuccess = false};
  }

  current_line_number++;

  std::vector<std::vector<Csv::CellReference>> cell_refs;

  parseLine(line, cell_refs);

  validateColumns(cell_refs);

  Record record = processColumns(cell_refs);

  return {.record = record, .isSuccess = true};
}

void DataParser::parseLine(
    const std::string &line,
    std::vector<std::vector<Csv::CellReference>> &cell_refs) const {
  try {
    std::string_view data(line);
    csv_parser.parseTo(data, cell_refs);
  } catch (Csv::ParseError &ex) {
    throw DataParserException(
        SimpleLang::Error(
            Error::CSVParsingError,
            {{"line_number", std::to_string(current_line_number)}}) +
        ": " + ex.what());
  }
}

void DataParser::validateColumns(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs) const {
  if (cell_refs.empty()) {
    throw DataParserException(SimpleLang::Error(
        Error::CSVParsingErrorEmptyLine,
        {{"line_number", std::to_string(current_line_number)}}));
  }

  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  const auto &network_params = manager.network_params;

  if (app_params.mode != EMode::Predict &&
      cell_refs.size() !=
          network_params.input_size + network_params.output_size) {
    throw DataParserException(SimpleLang::Error(
        Error::CSVParsingErrorColumnsSize,
        {{"line_number", std::to_string(current_line_number)},
         {"value", std::to_string(cell_refs.size())},
         {"expected", std::to_string(network_params.input_size +
                                     network_params.output_size)}}));
  }

  if (app_params.mode == EMode::Predict &&
      cell_refs.size() < network_params.input_size) {
    throw DataParserException(SimpleLang::Error(
        Error::CSVParsingErrorColumnsMin,
        {{"line_number", std::to_string(current_line_number)},
         {"value", std::to_string(cell_refs.size())},
         {"expected", std::to_string(network_params.input_size)}}));
  }
}

Record DataParser::processColumns(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs) const {
  Record record;
  try {
    const auto &manager = Manager::getInstance();
    const auto &app_params = manager.app_params;
    const auto &network_params = manager.network_params;

    if (app_params.mode == EMode::Predict &&
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
    throw DataParserException(SimpleLang::Error(
        Error::CSVParsingErrorColumnsBadFormat,
        {{"line_number", std::to_string(current_line_number)}}));
  }
  return record;
}

Record DataParser::processInputOnly(
    const std::vector<std::vector<Csv::CellReference>> &cell_refs,
    size_t input_size) const {
  std::vector<float> inputs(input_size);

  std::transform(cell_refs.begin(), cell_refs.begin() + input_size,
                 inputs.begin(), getFloatValue);
  return {.inputs = inputs, .outputs = std::vector<float>{}};
}

Record DataParser::processInputFirst(
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

Record DataParser::processOutputFirst(
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