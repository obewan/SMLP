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
  Testing(std::shared_ptr<Network> network,
          std::shared_ptr<DataFileParser> fileparser)
      : network_(network), fileParser_(fileparser),
        testingResults_(std::make_shared<TestingResult>()) {}

  /**
   * @brief Constructor that takes a pointer to the network and the file
   * path to testing data as arguments.
   *
   * @param network Pointer to the network.
   * @param file_path File path to the testing data.
   */
  Testing(std::shared_ptr<Network> network, const std::string &file_path)
      : network_(network),
        fileParser_(std::make_shared<DataFileParser>(file_path)),
        testingResults_(std::make_shared<TestingResult>()) {}

  /**
   * @brief This method tests the model with the given parameters.
   *
   * @param network_params Network parameters.
   * @param app_params Application parameters.
   * @param epoch The current epoch (default is 0).
   */
  void test(const NetworkParameters &network_params,
            const AppParameters &app_params, size_t epoch = 0);

  /**
   * @brief For testing a with a stdin pipe
   *
   */
  void testLines(const NetworkParameters &network_params,
                 const AppParameters &app_params, bool from_ratio_line,
                 size_t current_line_number);

  /**
   * @brief Sets the network for testing.
   *
   * @param network Pointer to the network.
   */
  void setNetwork(std::shared_ptr<Network> network) { network_ = network; }

  /**
   * @brief Gets the network used for testing.
   *
   * @return Pointer to the network.
   */
  std::shared_ptr<Network> getNetwork() const { return network_; }

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

  /**
   * @brief Get the Testing Results object
   *
   * @return std::shared_ptr<TestingResult>
   */
  std::shared_ptr<TestingResult> getTestingResults() const {
    return testingResults_;
  }

private:
  void testLine(const NetworkParameters &network_params,
                const AppParameters &app_params,
                const RecordResult &record_result, const size_t line_number,
                std::vector<TestingResult::TestResults> &testResults) const;

  std::shared_ptr<Network> network_ = nullptr;
  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
  std::shared_ptr<TestingResult> testingResults_ = nullptr;
};