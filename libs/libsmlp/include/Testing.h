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

namespace smlp {
enum class TestingType { TestingFile, TestingSocket, TestingStdin };

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
   */
  explicit Testing(TestingType testing_type)
      : testingType(testing_type),
        testingResults_(std::make_shared<TestingResult>()){};
  virtual ~Testing() = default;

  const TestingType testingType;

  /**
   * @brief Test a network
   *
   * @param line optional data line to use for testing
   * @param epoch optional epoch to indicate for testing results
   * @param current_line_number optional line number to indicate
   */
  virtual smlp::Result test(const std::string &line = "", size_t epoch = 0,
                            size_t current_line_number = 0) = 0;

  /**
   * @brief Create a File Parser object
   *
   */
  void createFileParser() {
    if (!dataParser_) {
      if (testingType == TestingType::TestingFile) {
        dataParser_ = std::make_shared<DataFileParser>();
      } else {
        dataParser_ = std::make_shared<DataParser>();
      }
    }
  }

  /**
   * @brief Sets the data parser for testing data.
   *
   * @param dataparser Pointer to the data parser.
   */
  void setDataParser(std::shared_ptr<DataParser> dataparser) {
    dataParser_ = dataparser;
  }

  /**
   * @brief Gets the data parser used for testing data.
   *
   * @return Pointer to the data parser.
   */
  std::shared_ptr<DataParser> getDataParser() const { return dataParser_; }

  std::shared_ptr<TestingResult> getTestingResults() const {
    return testingResults_;
  }

protected:
  TestingResult::TestResults testLine(const smlp::RecordResult &record_result,
                                      const size_t line_number = 0,
                                      const size_t epoch = 0) const;

  std::shared_ptr<DataParser> dataParser_ = nullptr;
  std::shared_ptr<TestingResult> testingResults_ = nullptr;
};
} // namespace smlp