/**
 * @file Manager.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief main manager
 * @date 2023-12-10
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once

#include "Common.h"
#include "NetworkImportExportJSON.h"
#include "Predict.h"
#include "Training.h"
#include <memory>

class Manager {
public:
  static Manager &getInstance() {
    static Manager instance;
    return instance;
  }
  Manager(Manager const &) = delete;
  void operator=(Manager const &) = delete;

  /**
   * @brief This method apply the model on inputs to predict the outputs
   * @param line Optional line to use.
   */
  void predict(const std::string &line = "");

  /**
   * @brief This method trains the model.
   * @param line Optional line to use.
   */
  void train(const std::string &line = "");

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
   * @brief Process line from TCP Client socket.
   *
   * @param line
   */
  void processTCPClient(const std::string &line);

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
   * @brief Network builder.
   */
  void importOrBuildNetwork();

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

private:
  Manager() = default;
  std::string showInlineHeader() const;
  std::unique_ptr<Predict> predict_ = nullptr;
  std::unique_ptr<Training> training_ = nullptr;
  std::unique_ptr<Testing> testing_ = nullptr;
};