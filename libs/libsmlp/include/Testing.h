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

enum class TestingType { TestingFile, TestingSocket, TestingStdin };

const std::map<std::string, TestingType, std::less<>> testing_map{
    {"TestingFile", TestingType::TestingFile},
    {"TestingSocket", TestingType::TestingSocket},
    {"TestingStdin", TestingType::TestingStdin}};

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
  explicit Testing(TestingType testing_type, const AppParameters &app_params)
      : testingType(testing_type), app_params_(app_params),
        testingResults_(std::make_shared<TestingResult>(app_params)){};
  virtual ~Testing() = default;

  TestingType testingType;

  const std::string UndefinedTesting = "UndefinedTesting";

  /**
   * @brief Test a network
   *
   * @param line optional data line to use for testing
   * @param epoch optional epoch to indicate for testing results
   * @param current_line_number optional line number to indicate
   */
  virtual void test(const std::string &line = "", size_t epoch = 0,
                    size_t current_line_number = 0) = 0;

  /**
   * @brief Create a File Parser object
   *
   */
  void createFileParser() {
    if (!fileParser_) {
      fileParser_ = std::make_shared<DataFileParser>();
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
    fileParser_ = std::make_shared<DataFileParser>();
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

  std::string testingTypeStr() const {
    for (const auto &[key, mTestingType] : testing_map) {
      if (mTestingType == testingType) {
        return key;
      }
    }
    return UndefinedTesting;
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