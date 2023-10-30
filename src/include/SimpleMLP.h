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

/**
 * @brief Main class to train and test the neural network after parsing the
 * arguments.
 *
 */
class SimpleMLP {
public:
  bool init(int argc, char **argv, bool &showVersion);

  /**
   * @brief This method trains the model.
   */
  void train();

  /**
   * @brief This method tests the model.
   */
  void test();

  /**
   * @brief This method trains the model, testing at each epoch and monitoring
   * the progress of an output neuron. Be aware that this mode consumes more
   * memory with each epoch to save the monitoring progress. Therefore, it is
   * recommended for use with smaller datasets and a lower number of epochs.
   */
  void trainTestMonitored();

  /**
   * @brief Application parameters.
   *
   */
  AppParameters app_params;

  /**
   * @brief Network parameters.
   */
  NetworkParameters network_params;

private:
  int parseArgs(int argc, char **argv, bool &showVersion);
  Network *network_ = nullptr;
};