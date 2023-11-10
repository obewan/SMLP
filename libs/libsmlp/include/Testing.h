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
#include "exception/TestingException.h"
#include <map>

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
  Testing(Network *network, DataFileParser *fileparser)
      : network_(network), fileParser_(fileparser) {}

  /**
   * @brief Constructor that takes a pointer to the network and the file path to
   * testing data as arguments.
   *
   * @param network Pointer to the network.
   * @param file_path File path to the testing data.
   */
  Testing(Network *network, const std::string &file_path)
      : network_(network), fileParser_(new DataFileParser(file_path)) {}

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
   * @brief TestResults structure that holds the epoch, line, expected output,
   * and actual output of a test.
   */
  struct TestResults {
    size_t epoch = 0;
    size_t line = 0;
    float expected = 0.0f;
    float output = 0.0f;
  };

  /**
   * @brief TestResultsExt structure that extends TestResults with a progress
   * vector, which can be used to monitor the progress of the test.
   */
  struct TestResultsExt {
    size_t epoch = 0;
    size_t line = 0;
    float expected = 0.0f;
    float output = 0.0f;
    std::vector<float> progress;
  };

  /**
   * @brief Stat structure that holds various statistics related to the test
   * results, such as total samples, correct predictions, good convergence,
   * total expected values, and accuracy.
   */
  struct Stat {
    size_t total_samples = 0;
    size_t correct_predictions_low = 0;
    size_t correct_predictions = 0;
    size_t correct_predictions_high = 0;
    size_t good_convergence = 0;
    size_t good_convergence_zero = 0;
    size_t good_convergence_one = 0;
    size_t total_expected_zero = 0;
    size_t total_expected_one = 0;

    float accuracy = 0.0f;
    float accuracy_low = 0.0f;
    float accuracy_high = 0.0f;
    float convergence = 0.0f;
    float convergence_zero = 0.0f;
    float convergence_one = 0.0f;
  };

  /**
   * @brief Displays a summary of the test results on a single line.
   */
  std::string showResultsLine();

  /**
   * @brief Displays detailed test results. If the verbose parameter is set to
   * true, additional information will be displayed.
   *
   * @param mode Mode of the training.
   * @param verbose If true, additional details are displayed (default is
   * false).
   */
  std::string showResults(EMode mode, bool verbose = false);

  /**
   * @brief Display some verbose results.
   *
   */
  std::string showResultsVerbose(const TestResultsExt &result,
                                 EMode mode) const;

  /**
   * @brief Calculates and returns the statistics of the test results.
   *
   * @return A Stat object containing the calculated statistics.
   */
  Stat calcStats();

  /**
   * @brief Sets the network for testing.
   *
   * @param network Pointer to the network.
   */
  void setNetwork(Network *network) { network_ = network; }

  /**
   * @brief Gets the network used for testing.
   *
   * @return Pointer to the network.
   */
  Network *getNetwork() { return network_; }

  /**
   * @brief Sets the file parser for testing data.
   *
   * @param fileparser Pointer to the file parser.
   */
  void setFileParser(DataFileParser *fileparser) { fileParser_ = fileparser; }

  /**
   * @brief Gets the file parser used for testing data.
   *
   * @return Pointer to the file parser.
   */
  DataFileParser *getFileParser() { return fileParser_; }

  std::vector<TestResultsExt> testResultExts;
  std::map<size_t, std::vector<float>> progress;

private:
  Network *network_;
  DataFileParser *fileParser_;
  std::vector<TestResults> lastEpochTestResultTemp_;
  size_t last_epoch_ = 0;
};