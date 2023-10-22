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
#include <fstream>

class FileParser {
public:
  explicit FileParser(const std::string &path) : path_(path){};
  virtual ~FileParser();
  bool OpenFile();
  void CloseFile();
  void ResetPos();

  RecordResult ProcessLine(const Parameters &params);

  Record ProcessInputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t input_size) const;
  Record ProcessOutputFirst(
      const std::vector<std::vector<Csv::CellReference>> &cell_refs,
      size_t output_size) const;
  std::ifstream file_;
  std::string path_;
  Csv::Parser parser_;
  size_t line_number = 0;
};