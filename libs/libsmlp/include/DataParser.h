/**
 * @file DataParser.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Data Parser
 * @date 2024-02-20
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "../../csv-parser/include/csv_parser.h"
#include "Common.h"
#include "CommonModes.h"
#include "CommonParameters.h"
#include "exception/DataParserException.h"
#include <cstddef>
#include <functional>

namespace smlp {
enum class DataParserType { DataParser, DataFileParser };

class DataParser {
public:
  explicit DataParser(DataParserType data_type = DataParserType::DataParser)
      : dataType(data_type) {}
  virtual ~DataParser() = default;

  const DataParserType dataType;

  size_t current_line_number = 0;
  size_t total_lines = 0;
  size_t training_ratio_line = 0;
  bool isTrainingRatioLineProcessed = false;
  Csv::Parser csv_parser;

  /**
   * @brief Processes a line returns a RecordResult. This
   * method can be used for both testing and training data.

   * @param line the raw line to process or the next line if empty
   * @param isTesting testing flag to skip some training data
   * @return A RecordResult containing the processed data from the line.
   */
  virtual RecordResult processLine(const std::string &line = "",
                                   bool isTesting = false);

  /**
   * @brief Processes a record with input only.
   * @param cell_refs A 2D vector containing Csv::CellReference objects for each
   * cell in the record.
   * @param input_size The size of the input data in the record.
   * @return A Record containing the processed data from the record.
   */
  virtual Record processInputOnly(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size) const;

  /**
   * @brief Processes a record with input first. This method is used when the
   * input values are located before the output values in a record.
   *
   * @param cell_refs A 2D vector containing Csv::CellReference objects for each
   * cell in the record.
   * @param input_size The size of the input data in the record.
   * @param output_size The size of the output data in the record.
   * @return A Record containing the processed data from the record.
   */
  virtual Record processInputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size, size_t output_size) const;

  /**
   * @brief Processes a record with output first. This method is used when the
   * output values are located before the input values in a record.
   *
   * @param cell_refs A 2D vector containing Csv::CellReference objects for each
   * cell in the record.
   * @param input_size The size of the input data in the record.
   * @param output_size The size of the output data in the record.
   * @return A Record containing the processed data from the record.
   */
  virtual Record processOutputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size, size_t output_size) const;

  virtual void
  parseLine(const std::string &line,
            std::vector<std::vector<Csv::CellReference>> &cell_refs) const;

  virtual void validateColumns(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs) const;

  virtual Record processColumns(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs) const;

  // lambda function to convert to float (this will throw if no value)
  std::function<float(const std::vector<Csv::CellReference> &)> getFloatValue =
      [](const std::vector<Csv::CellReference> &cells) {
        return static_cast<float>(cells[0].getDouble().value());
      };
};

} // namespace smlp