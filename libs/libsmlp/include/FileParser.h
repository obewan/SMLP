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
#include <cstddef>
#include <fstream>

using RecordFunction =
    std::function<void(std::pair<std::vector<float>, std::vector<float>>)>;

class FileParser {
public:
  explicit FileParser(const std::string &path) : path_(path){};
  virtual ~FileParser();
  bool OpenFile();
  void CloseFile();
  void ResetPos();
  bool ProcessFile(size_t from_line, size_t to_line, size_t input_size,
                   size_t output_size, bool output_at_end,
                   const RecordFunction &processRecord);
  bool ProcessLine(const std::string &line, size_t line_number,
                   size_t input_size, size_t output_size, bool output_at_end,
                   const RecordFunction &processRecord) const;

private:
  std::pair<std::vector<float>, std::vector<float>> ProcessInputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size) const;
  std::pair<std::vector<float>, std::vector<float>> ProcessOutputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t output_size) const;
  std::ifstream file_;
  std::string path_;
  Csv::Parser parser_;
};