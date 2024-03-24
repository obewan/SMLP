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
#include "DataParser.h"
#include <fstream>

namespace smlp {
/**
 * @brief The FileParser class is responsible for parsing a file. It contains
 * methods for opening, closing, and resetting the file position, as well as
 * methods for processing lines and counting lines.
 */
class DataFileParser : public DataParser {
public:
  explicit DataFileParser() : DataParser(DataParserType::DataFileParser) {}
  ~DataFileParser() override;

  RecordResult processLine(const std::string &line = "",
                           bool isTesting = false) override;

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
        (size_t)std::count(std::istreambuf_iterator<char>((std::istream &)file),
                   std::istreambuf_iterator<char>(), '\n');
    closeFile();
    return total_lines;
  }

  bool getNextLine(std::string &line, bool isTesting);

  std::ifstream file;
};
} // namespace smlp