/**
 * @file SimpleMLP.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Main program header
 * @date 2023-09-10
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Monitor.h"
#include "Network.h"
#include <string>

struct Parameters {
  std::string title;
  std::string data_file;
  size_t input_size;
  size_t hidden_size;
  size_t output_size;
  size_t num_epochs;
  size_t to_line;
  float learning_rate;
  float beta1;
  float beta2;
  bool output_at_end;
};

int parseArgs(int argc, char *argv[], Parameters &params);

Monitor *buildMonitor(Network *network);