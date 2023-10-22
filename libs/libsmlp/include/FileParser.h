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
#include <cstddef>
#include <fstream>

using RecordFunction = std::function<void(
    size_t, size_t, std::pair<std::vector<float>, std::vector<float>>)>;

class FileParser {
public:
  explicit FileParser(const std::string &path) : path_(path){};
  virtual ~FileParser();
  bool OpenFile();
  void CloseFile();
  void ResetPos();

  RecordResult ProcessLine(const Parameters &params);

  size_t line_number;

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