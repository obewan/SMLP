/**
 * @file Predict.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Predict class
 * @date 2023-11-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "DataFileParser.h"
#include "Network.h"
#include "SimpleLogger.h"
#include "exception/PredictException.h"
#include <memory>

/**
 * @brief Predict class for predictive operations.
 */
class Predict {
public:
  Predict(std::shared_ptr<Network> network,
          std::shared_ptr<DataFileParser> fileparser,
          const AppParameters &app_params, const SimpleLogger &logger)
      : network_(network), fileParser_(fileparser), app_params_(app_params),
        logger_(logger) {}

  Predict(std::shared_ptr<Network> network, const AppParameters &app_params,
          const SimpleLogger &logger)
      : network_(network),
        fileParser_(std::make_shared<DataFileParser>(app_params.data_file)),
        app_params_(app_params), logger_(logger) {}

  void predict() const;

  void appendValues(const std::vector<float> &values, bool roundValues) const;

  void showOutput(const std::vector<float> &inputs,
                  const std::vector<float> &predicteds) const;

  /**
   * @brief Sets the network for testing.
   *
   * @param network Pointer to the network.
   */
  void setNetwork(std::shared_ptr<Network> network) { network_ = network; }

  /**
   * @brief Gets the network used for testing.
   *
   * @return Pointer to the network.
   */
  std::shared_ptr<Network> getNetwork() const { return network_; }

  /**
   * @brief Sets the file parser for testing data.
   *
   * @param fileparser Pointer to the file parser.
   */
  void setFileParser(std::shared_ptr<DataFileParser> fileparser) {
    fileParser_ = fileparser;
  }

  /**
   * @brief Gets the file parser used for testing data.
   *
   * @return Pointer to the file parser.
   */
  std::shared_ptr<DataFileParser> getFileParser() const { return fileParser_; }

private:
  std::shared_ptr<Network> network_ = nullptr;
  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
  const AppParameters &app_params_;
  const SimpleLogger &logger_;
};