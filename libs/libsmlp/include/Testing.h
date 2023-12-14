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
   * @param app_params
   */
  explicit Testing(const AppParameters &app_params)
      : app_params_(app_params),
        testingResults_(std::make_shared<TestingResult>(app_params)){};
  virtual ~Testing() = default;

  virtual void test(size_t epoch = 0, size_t current_line = 0) = 0;

  /**
   * @brief Create a File Parser object
   *
   */
  void createFileParser() {
    if (!fileParser_) {
      fileParser_ = std::make_shared<DataFileParser>(app_params_.data_file);
    }
  }

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

  /**
   * @brief Set the Network object
   *
   * @param network
   */
  void setNetwork(std::shared_ptr<Network> network) { network_ = network; }

  std::shared_ptr<TestingResult> getTestingResults() const {
    return testingResults_;
  }

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

  std::shared_ptr<Network> network_ = nullptr;
  const AppParameters &app_params_;
  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
  std::shared_ptr<TestingResult> testingResults_ = nullptr;
};