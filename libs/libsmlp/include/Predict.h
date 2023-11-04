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
#include "FileParser.h"
#include "Network.h"
#include "SimpleLogger.h"
#include "exception/PredictException.h"

/**
 * @brief Predict class for predictive operations.
 */
class Predict {
public:
  Predict(Network *network, FileParser *fileparser,
          const AppParameters &app_params, const SimpleLogger &logger)
      : network_(network), fileParser_(fileparser), app_params_(app_params),
        logger_(logger) {}

  Predict(Network *network, const AppParameters &app_params,
          const SimpleLogger &logger)
      : network_(network), fileParser_(new FileParser(app_params.data_file)),
        app_params_(app_params), logger_(logger) {}

  void predict();

  void appendValues(const std::vector<float> &values, bool roundValues) const;

  void showOutput(const std::vector<float> &inputs,
                  const std::vector<float> &predicteds) const;

  /**
   * @brief Sets the network for testing.
   *
   * @param network Pointer to the network.
   */
  void setNetwork(Network *network) { network_ = network; }

  /**
   * @brief Gets the network used for testing.
   *
   * @return Pointer to the network.
   */
  Network *getNetwork() { return network_; }

  /**
   * @brief Sets the file parser for testing data.
   *
   * @param fileparser Pointer to the file parser.
   */
  void setFileParser(FileParser *fileparser) { fileParser_ = fileparser; }

  /**
   * @brief Gets the file parser used for testing data.
   *
   * @return Pointer to the file parser.
   */
  FileParser *getFileParser() { return fileParser_; }

private:
  Network *network_;
  FileParser *fileParser_;
  const AppParameters &app_params_;
  const SimpleLogger &logger_;
};