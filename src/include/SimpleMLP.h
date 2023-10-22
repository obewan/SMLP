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
#include "Common.h"
#include "Network.h"
#include <string>

class SimpleMLP {
public:
  bool init(int argc, char **argv, bool withMonitoring = true);
  void train();
  void test();
  void trainAndTest();

private:
  // default parameters

  Parameters params_;

  int parseArgs(int argc, char **argv);
  Network *network_ = nullptr;
};