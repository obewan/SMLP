#include "DataFileParser.h"
#include "Common.h"
#include "CommonErrors.h"
#include "Manager.h"
#include "SimpleLang.h"
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
    throw DataParserException(SimpleLang::Error(Error::FailedToOpenFile));
  }
  file.open(data_file);
  if (!file.is_open()) {
    throw DataParserException(SimpleLang::Error(Error::FailedToOpenFile) +
                              ": " + data_file);
  }
  current_line_number = 0;
}

void DataFileParser::closeFile() {
  if (file.is_open()) {
    file.close();
  }
  current_line_number = 0;
}

void DataFileParser::resetPos() {
  file.clear();
  file.seekg(0, std::ios::beg);
  current_line_number = 0;
}

RecordResult DataFileParser::processLine(const std::string &line,
                                         bool isTesting) {
  std::string nextline;
  current_line_number++;
  if (!line.empty()) {
    nextline = line;
  } else if (!getNextLine(nextline, isTesting)) {
    return {.isSuccess = false, .isEndOfFile = true};
  }

  std::vector<std::vector<Csv::CellReference>> cell_refs;

  parseLine(nextline, cell_refs);

  validateColumns(cell_refs);

  Record record = processColumns(cell_refs);

  return {.record = record, .isSuccess = true};
}

bool DataFileParser::getNextLine(std::string &line, bool isTesting) {
  // if isTesting, skipping lines until testing lines
  const auto &app_params = Manager::getInstance().app_params;
  if (isTesting && app_params.use_training_ratio_line &&
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
