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

class SimpleMLP {
public:
  bool init(int argc, char **argv, bool withMonitoring = true);
  bool train();
  bool test();

private:
  // default parameters
  struct Parameters {
    std::string title;
    std::string data_file;
    size_t input_size = 0;
    size_t hidden_size = 10;
    size_t output_size = 1;
    size_t num_epochs = 3;
    size_t to_line = 0;
    float learning_rate = 1e-3f;
    float beta1 = 0.9f;
    float beta2 = 0.99f;
    bool output_at_end = false;
  };

  Parameters params_ = {
      .title = "SMLP",
      .data_file = "",
  };

  int parseArgs(int argc, char **argv);
  Monitor *buildMonitor(Network *network);
  Optimizer *optimizer_ = nullptr;
  Network *network_ = nullptr;
};