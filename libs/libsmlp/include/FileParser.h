/**
 * @file FileParser.h
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
#include "exception/FileParserException.h"
#include <cstddef>
#include <fstream>

class FileParser {
public:
  explicit FileParser(const std::string &path) : path(path) {}

  virtual ~FileParser();
  void openFile();
  void closeFile();
  void resetPos();

  size_t getTrainingRatioLine(float trainingRatio) {
    total_lines = countLine();
    training_ratio_line = (size_t)((float)total_lines * trainingRatio);
    return training_ratio_line;
  }

  std::size_t countLine() {
    openFile();
    total_lines =
        std::count(std::istreambuf_iterator<char>((std::istream &)file),
                   std::istreambuf_iterator<char>(), '\n');
    closeFile();
    return total_lines;
  }

  RecordResult processLine(const Parameters &params, bool isTesting = false);

  Record processInputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size) const;
  Record processOutputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t output_size) const;
  std::ifstream file;
  std::string path;
  Csv::Parser csv_parser;
  size_t current_line_number = 0;
  size_t total_lines = 0;
  size_t training_ratio_line = 0;
};
