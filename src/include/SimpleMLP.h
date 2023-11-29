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
#include "NetworkImportExportJSON.h"
#include "SimpleConfig.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include <memory>
#include <string>

/**
 * @brief Main class to train and test the neural network after parsing the
 * arguments.
 *
 */
class SimpleMLP {
public:
  SimpleMLP() { logger = std::make_shared<SimpleLogger>(); }

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
   * @brief This method apply the model on inputs to predict the outputs
   */
  void predict();

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
   * @brief run the selected mode.
   */
  void runMode();

  /**
   * @brief check if the network should export its model.
   */

  bool shouldExportNetwork() const;

  /**
   * @brief export the network model.
   */
  void exportNetwork();

  /**
   * @brief Application parameters.
   */
  AppParameters app_params;

  /**
   * @brief Network parameters.
   */
  NetworkParameters network_params;

  /**
   * @brief The neural network
   */
  std::shared_ptr<Network> network = nullptr;

  /**
   * @brief ImportExportJSON tool.
   */
  NetworkImportExportJSON importExportJSON;

  /**
   * @brief A simple logger
   */
  std::shared_ptr<SimpleLogger> logger = nullptr;

private:
  int parseArgs(int argc, char **argv);
  std::string showInlineHeader() const;
  void ConfigSettings(const SimpleConfig &config);
  void importOrBuildNetwork();
  void checkStdin();
};