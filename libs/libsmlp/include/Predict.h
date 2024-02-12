/**
 * @file Predict.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Predict class
 * @date 2023-11-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "DataFileParser.h"
#include "Network.h"
#include "exception/PredictException.h"
#include <memory>

/**
 * @brief Predict class for predictive operations.
 */
class Predict {
public:
  explicit Predict(std::shared_ptr<DataFileParser> fileparser)
      : fileParser_(fileparser) {}

  Predict() : fileParser_(std::make_shared<DataFileParser>()) {}

  std::string predict(const std::string &line = "") const;

  std::string formatValues(const std::vector<float> &values,
                           bool roundValues) const;

  std::string formatResult(const std::vector<float> &inputs,
                           const std::vector<float> &predicteds) const;

  /**
   * @brief Sets the file parser for testing data.
   *
   * @param fileparser Pointer to the file parser.
   */
  void setFileParser(std::shared_ptr<DataFileParser> fileparser) {
    fileParser_ = fileparser;
  }

  /**
   * @brief Gets the file parser used for testing data.
   *
   * @return Pointer to the file parser.
   */
  std::shared_ptr<DataFileParser> getFileParser() const { return fileParser_; }

private:
  std::string processInput(EInput input, const std::string &line) const;
  std::string processLine(const Common::RecordResult &result) const;
  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
};