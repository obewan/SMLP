/**
 * @file Common.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common tools and objects
 * @date 2023-10-22
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <string>
#include <vector>

/**
 * @brief Record of inputs vector and outputs vector, in that order.
 *
 */
using Record = std::pair<std::vector<float>, std::vector<float>>;

struct Parameters {
  std::string title = "SMLP";
  std::string data_file = "";
  size_t input_size = 0;
  size_t hidden_size = 10;
  size_t output_size = 1;
  size_t hiddens_count = 1;
  size_t num_epochs = 3;
  size_t from_line = 0;
  size_t to_line = 0;
  float learning_rate = 1e-3f;
  bool output_at_end = false;
  bool verbose = false;
};

struct RecordResult {
  bool isSuccess = false;
  bool isLineSkipped = false;
  bool isEndOfFile = false;
  bool isEndOfTrainingLines = false;
  Record record;
};