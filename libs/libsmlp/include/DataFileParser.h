/**
 * @file DataFileParser.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief File Parser
 * @date 2023-09-01
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "../../csv-parser/include/csv_parser.h"
#include "Common.h"
#include "CommonModes.h"
#include "CommonParameters.h"
#include <cstddef>
#include <fstream>
#include <functional>

namespace smlp {
/**
 * @brief The FileParser class is responsible for parsing a file. It contains
 * methods for opening, closing, and resetting the file position, as well as
 * methods for processing lines and counting lines.
 */
class DataFileParser {
public:
  /**
   * @brief Constructor
   */
  DataFileParser() = default;

  // Virtual destructor
  virtual ~DataFileParser();

  /**
   * @brief Opens the file.
   */
  void openFile();

  /**
   * @brief Closes the file.
   */
  void closeFile();

  /**
   * @brief Resets the file position to the beginning of the file.
   */
  void resetPos();

  /**
   * @brief Calculates the line number that corresponds to the given training
   * ratio.
   *
   * @param app_params The application parameters.
   */
  void calcTrainingRatioLine(const AppParameters &app_params) {
    if (app_params.input != EInput::File ||
        app_params.training_ratio_line > 0) {
      training_ratio_line = app_params.training_ratio_line;
    } else {
      total_lines = countLine();
      training_ratio_line =
          (size_t)((float)total_lines * app_params.training_ratio);
    }
    isTrainingRatioLineProcessed = true;
  }

  /**
   * @brief Counts the total number of lines in the file.
   *
   * @return The total number of lines in the file.
   */
  std::size_t countLine() {
    openFile();
    total_lines =
        std::count(std::istreambuf_iterator<char>((std::istream &)file),
                   std::istreambuf_iterator<char>(), '\n');
    closeFile();
    return total_lines;
  }

  /**
   * @brief Processes a line from the file and returns a RecordResult. This
   * method can be used for both testing and training data.

   * @param line if not empty it will use this line to process, else it will
   * process the next line of the fileparser.
   * @param isTesting testing flag to skip some training data
   * @return A RecordResult containing the processed data from the line.
   */
  smlp::RecordResult processLine(const std::string &line = "",
                                 bool isTesting = false);

  /**
   * @brief Processes a record with input only.
   * @param cell_refs A 2D vector containing Csv::CellReference objects for each
   * cell in the record.
   * @param input_size The size of the input data in the record.
   * @return A Record containing the processed data from the record.
   */
  smlp::Record processInputOnly(
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
  smlp::Record processInputFirst(
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
  smlp::Record processOutputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size, size_t output_size) const;

  bool getNextLine(std::string &line, bool isTesting);

  void parseLine(const std::string &line,
                 std::vector<std::vector<Csv::CellReference>> &cell_refs) const;

  void validateColumns(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs) const;

  smlp::Record processColumns(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs) const;

  std::ifstream file;
  Csv::Parser csv_parser;
  size_t current_line_number = 0;
  size_t total_lines = 0;
  size_t training_ratio_line = 0;
  bool isTrainingRatioLineProcessed = false;

  std::function<float(const std::vector<Csv::CellReference> &)> getFloatValue =
      [](const std::vector<Csv::CellReference> &cells) {
        return (float)cells[0].getDouble().value();
      };
};
} // namespace smlp