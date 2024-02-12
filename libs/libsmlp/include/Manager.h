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
#include "CommonModes.h"
#include "NetworkImportExportJSON.h"
#include "Predict.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "TestingFile.h"
#include "TestingSocket.h"
#include "TestingStdin.h"
#include "TrainingFile.h"
#include "TrainingSocket.h"
#include "TrainingStdin.h"
#include "exception/ManagerException.h"
#include <algorithm>
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
  std::string predict(const std::string &line = "");

  /**
   * @brief This method trains the model.
   * @param line Optional line to use.
   */
  void train(const std::string &line = "");

  /**
   * @brief This method tests the model.
   * @param line Optional line to use.
   */
  void test(const std::string &line = "");

  /**
   * @brief This method trains the model, testing at each epoch and monitoring
   * the progress of an output neuron. Be aware that this mode consumes more
   * memory with each epoch to save the monitoring progress. Therefore, it is
   * recommended for use with smaller datasets and a lower number of epochs.
   * @param line Optional line to use.
   */
  void trainTestMonitored(const std::string &line = "");

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

  /**
   * @brief Create a Training object
   *
   */
  void createTraining() {
    if (training_) {
      return;
    }
    switch (app_params.input) {
    case EInput::File:
      training_ = std::make_shared<TrainingFile>();
      break;
    case EInput::Stdin:
      training_ = std::make_shared<TrainingStdin>();
      break;
    case EInput::Socket:
      training_ = std::make_shared<TrainingSocket>();
      break;
    default:
      throw ManagerException(SimpleLang::Error(Error::UnimplementedMode));
    }

    training_->createFileParser();
    if (app_params.mode == EMode::TrainTestMonitored) {
      createTesting();
      testing_->setFileParser(training_->getFileParser());
    }
  }

  /**
   * @brief Create a Testing object
   *
   */
  void createTesting() {
    if (testing_) {
      return;
    }
    switch (app_params.input) {
    case EInput::File:
      testing_ = std::make_shared<TestingFile>();
      break;
    case EInput::Stdin:
      testing_ = std::make_shared<TestingStdin>();
      break;
    case EInput::Socket:
      testing_ = std::make_shared<TestingSocket>();
      break;
    default:
      throw ManagerException(SimpleLang::Error(Error::UnimplementedMode));
    }

    if (app_params.mode != EMode::TrainTestMonitored) {
      // TrainTestMonitored use the file parser of training_ instead.
      testing_->createFileParser();
    }
  }

  /**
   * @brief This will delete the managed object if this is the last shared_ptr
   * owning it.
   *
   */
  void resetTraining() {
    if (training_ == nullptr) {
      return;
    }
    training_.reset();
  }

  /**
   * @brief This will delete the managed object if this is the last shared_ptr
   * owning it.
   *
   */
  void resetTesting() {
    if (testing_ == nullptr) {
      return;
    }
    testing_.reset();
  }

  /**
   * @brief Logger
   *
   */
  const SimpleLogger &logger = SimpleLogger::getInstance();

  /**
   * @brief Get the Training object
   *
   * @return std::shared_ptr<Training>
   */
  std::shared_ptr<Training> getTraining() const { return training_; }

  /**
   * @brief Get the Testing object
   *
   * @return std::shared_ptr<Testing>
   */
  std::shared_ptr<Testing> getTesting() const { return testing_; }

private:
  Manager() = default;
  std::string showInlineHeader() const;
  std::shared_ptr<Predict> predict_ = nullptr;
  std::shared_ptr<Training> training_ = nullptr;
  std::shared_ptr<Testing> testing_ = nullptr;
};