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
#include "CLI11.hpp"
#include "CommonParameters.h"
#include "Manager.h"
#include "SimpleConfig.h"
#include <memory>

/**
 * @brief Main class to train and test the neural network after parsing the
 * arguments.
 *
 */
class SimpleMLP {
public:
  SimpleMLP() = default;

  static const int EXIT_HELP = 2;
  static const int EXIT_VERSION = 3;

  /**
   * @brief Initialise the application
   *
   * @param argc command line arguments counter
   * @param argv command line arguments array
   * @return int error code, EXIT_SUCCESS (0) if success
   */
  int init(int argc, char **argv);

  /**
   * @brief Main run
   *
   */
  void run() {
    auto &manager = smlp::Manager::getInstance();
    manager.runMode();
  }

private:
  int parseArgs(int argc, char **argv);
  void addOptions(CLI::App &app, bool &version, smlp::AppParameters &app_params,
                  smlp::NetworkParameters &network_params);
  void ConfigSettings(const smlp::SimpleConfig &config);
  void checkStdin();
};
