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
#include "Network.h"
#include <cstddef>
#include <string>

class Testing {
public:
  Testing(Network *network, const std::string &test_data_file_path)
      : network_(network), test_data_file_path_(test_data_file_path) {}

  /**
   * @brief Test the neural network with some data
   *
   * @param ouput_at_end if true the output value must be at end of record, else
   * at start.
   * @param from_line start train from this line, must be lesser than total
   * lines.
   * @param to_line end train at this line (including), must be greater than
   * from_line or equal to 0 for end of file.
   * @return int EXIT_SUCCESS (0) if no error, else EXIT_FAILURE (1)
   */
  int Test(bool output_at_end = true, size_t from_line = 0, size_t to_line = 0);

private:
  Network *network_;
  std::string test_data_file_path_;
};