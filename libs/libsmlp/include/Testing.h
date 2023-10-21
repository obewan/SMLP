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
#include "FileParser.h"
#include "Network.h"
#include <cstddef>
#include <map>

class Testing : public FileParser {
public:
  Testing(Network *network, const std::string &file_path)
      : FileParser(file_path), network_(network) {}

  /**
   * @brief Test the neural network with some data
   *
   * @param ouput_at_end if true the output value must be at end of record, else
   * at start.
   * @param from_line start train from this line, must be lesser than total
   * lines.
   * @param to_line end train at this line (including), must be greater than
   * from_line or equal to 0 for end of file.
   * @return bool true if success
   */
  bool Test(bool output_at_end = true, size_t from_line = 0, size_t to_line = 0,
            size_t epoch = 0);

  struct TestResults {
    size_t epoch;
    size_t line;
    float expected;
    float output;
  };

  struct TestResultsExt {
    size_t epoch;
    size_t line;
    float expected;
    float output;
    std::vector<float> progress;
  };

  std::vector<TestResultsExt> testResultExts;
  std::map<int, std::vector<float>> progress;

  void showResults();

private:
  Network *network_;
  std::vector<TestResults> lastEpochTestResultTemp_;
  size_t last_epoch_ = 0;
};