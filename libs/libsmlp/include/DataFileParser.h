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
#include <cstddef>
#include <fstream>

/**
 * @brief The FileParser class is responsible for parsing a file. It contains
 * methods for opening, closing, and resetting the file position, as well as
 * methods for processing lines and counting lines.
 */
class DataFileParser {
public:
  /**
   * @brief Constructor that takes a file path as an argument.
   *
   * @param path The path to the file.
   */
  explicit DataFileParser(const std::string &path) : path(path) {}

  // Virtual destructor
  virtual ~DataFileParser();

  /**
   * @brief Opens the file.
   */
  void openFile(const std::string &filepath = "");

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
   * @param trainingRatio The ratio of training data to total data.
   * @param trainingRatioLine A training ratio line to use instead.
   * @param use_stdin if true this will use training_ratio_line
   * @return The line number that corresponds to the training ratio.
   */
  size_t getTrainingRatioLine(float trainingRatio, size_t trainingRatioLine,
                              bool use_stdin) {
    if (use_stdin) {
      training_ratio_line = trainingRatioLine;
    } else {
      total_lines = countLine();
      training_ratio_line = (size_t)((float)total_lines * trainingRatio);
    }
    isTrainingRatioLineProcessed = true;
    return training_ratio_line;
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
   *
   * @param network_params The parameters to use when processing the line.
   * @param app_params The application parameters.
   * @param line if not empty it will use this line to process, else it will
   * process the next line of the fileparser.
   * @return A RecordResult containing the processed data from the line.
   */
  RecordResult processLine(const NetworkParameters &network_params,
                           const AppParameters &app_params,
                           const std::string &line = "");

  /**
   * @brief Processes a record with input only.
   * @param cell_refs A 2D vector containing Csv::CellReference objects for each
   * cell in the record.
   * @param input_size The size of the input data in the record.
   * @return A Record containing the processed data from the record.
   */
  Record processInputOnly(
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
  Record processInputFirst(
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
  Record processOutputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size, size_t output_size) const;

  bool getNextLine(std::string &line, const AppParameters &app_params);
  void parseLine(const std::string &line,
                 std::vector<std::vector<Csv::CellReference>> &cell_refs) const;
  void
  validateColumns(const std::vector<std::vector<Csv::CellReference>> &cell_refs,
                  const NetworkParameters &network_params,
                  const AppParameters &app_params) const;

  Record
  processColumns(const std::vector<std::vector<Csv::CellReference>> &cell_refs,
                 const NetworkParameters &network_params,
                 const AppParameters &app_params) const;

  std::ifstream file;
  Csv::Parser csv_parser;
  size_t current_line_number = 0;
  size_t total_lines = 0;
  size_t training_ratio_line = 0;
  bool isTrainingRatioLineProcessed = false;
  std::string path = "";
};
