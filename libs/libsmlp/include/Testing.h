/**
 * @file Testing.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Testing
 * @date 2023-08-29
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "DataFileParser.h"
#include "Manager.h"
#include "Network.h"
#include "TestingResult.h"
#include "exception/TestingException.h"
#include <cstddef>
#include <memory>

/**
 * @brief The Testing class is responsible for testing the neural network model.
 * It contains methods for processing test data, calculating statistics, and
 * displaying test results.
 */
class Testing {
public:
  /**
   * @brief Constructor that takes a pointer to the network and a pointer to the
   * file parser as arguments.
   *
   * @param network Pointer to the network.
   * @param fileparser Pointer to the file parser.
   */
  Testing() = default;

  virtual void test(size_t epoch = 0) = 0;

  /**
   * @brief Sets the file parser for testing data.
   *
   * @param fileparser Pointer to the file parser.
   */
  void setFileParser(std::shared_ptr<DataFileParser> fileparser) {
    fileParser_ = fileparser;
  }

  /**
   * @brief Sets the file parser for testing data.
   *
   * @param filepath file path.
   */
  void setFileParser(const std::string &filepath) {
    fileParser_ = std::make_shared<DataFileParser>(filepath);
  }

  /**
   * @brief Gets the file parser used for testing data.
   *
   * @return Pointer to the file parser.
   */
  std::shared_ptr<DataFileParser> getFileParser() const { return fileParser_; }

protected:
  TestingResult::TestResults testLine(const RecordResult &record_result,
                                      const size_t line_number = 0,
                                      const size_t epoch = 0) const {
    auto predicteds = network_->forwardPropagation(record_result.record.inputs);
    return {.epoch = epoch,
            .lineNumber = line_number,
            .expecteds = record_result.record.outputs,
            .outputs = predicteds};
  };

  std::shared_ptr<Network> network_ = Manager::getInstance().network;
  const AppParameters &app_params = Manager::getInstance().app_params;
  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
};