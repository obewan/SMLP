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
    auto &manager = Manager::getInstance();
    manager.runMode();
    if (manager.shouldExportNetwork()) {
      manager.exportNetwork();
    }
  }

private:
  int parseArgs(int argc, char **argv);
  void ConfigSettings(const SimpleConfig &config);
  void checkStdin();
  AppParameters &app_params = Manager::getInstance().app_params;
  NetworkParameters &network_params = Manager::getInstance().network_params;
};