/**
 * @file Training.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Training
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "DataFileParser.h"
#include "Network.h"
#include "Testing.h"
#include "exception/TrainingException.h"
#include <memory>
#include <string>

/**
 * Here's a high-level overview of the process:

1. Forward Propagation:
Input data is fed into the network.
The network performs forward propagation through each layer to compute predicted
output values.

2. Compute Loss:
During the backward propagation phase, you compare the predicted output values
with the actual output values from your training data. The loss function (e.g.,
mean squared error) is used to measure the difference between predicted and
actual outputs.

3. Compute Gradients:
The gradients of the loss with respect to the network's parameters (weights and
biases) are computed through the layers using the backpropagation algorithm.

4. Update Weights and Biases:
The gradients are used to update the weights and biases of the network using an
optimization algorithm (e.g., Adam, SGD). The learning rate determines how large
the updates to the weights and biases will be. The process repeats for multiple
training examples and epochs, gradually improving the network's ability to
predict the correct outputs for the given inputs.
 *
 */

/**
 * @brief The Training class is responsible for training the neural network
 * model. It contains methods for processing training data, updating the model
 * parameters, and monitoring the progress of training.
 */
class Training {
public:
  // Default constructor
  Training() = default;

  /**
   * @brief Constructor that takes a pointer to the network and the file path to
   * training data as arguments.
   *
   * @param network Pointer to the network.
   * @param file_path File path to the training data.
   */
  Training(std::shared_ptr<Network> network, const std::string &file_path)
      : network_(network),
        fileParser_(std::make_shared<DataFileParser>(file_path)) {}

  /**
   * @brief This method trains the model with the given parameters.
   *
   * @param network_params Network parameters for training.
   * @param app_params Application parameters.
   * @param line Optional line to use.
   */
  void train(const NetworkParameters &network_params,
             const AppParameters &app_params, const std::string &line = "");

  /**
   * @brief Sets the network for training.
   *
   * @param network Pointer to the network.
   */
  void setNetwork(std::shared_ptr<Network> network) { network_ = network; }

  /**
   * @brief Gets the network used for training.
   *
   * @return Pointer to the network.
   */
  std::shared_ptr<Network> getNetwork() const { return network_; }

  /**
   * @brief Sets the file parser for training data.
   *
   * @param fileParser Pointer to the file parser.
   */
  void setFileParser(std::shared_ptr<DataFileParser> fileParser) {
    fileParser_ = fileParser;
  }

  /**
   * @brief Gets the file parser used for training data.
   *
   * @return Pointer to the file parser.
   */
  std::shared_ptr<DataFileParser> getFileParser() const { return fileParser_; }

  /**
   * @brief Sets the tester for testing during training.
   *
   * @param tester Pointer to the tester.
   */
  void setTesting(std::shared_ptr<Testing> tester) { testing_ = tester; }

  /**
   * @brief Create a Testing object
   *
   * @param network_params
   * @param app_params
   */
  void createTesting() {
    testing_ = std::make_shared<Testing>(network_, fileParser_);
  }

  /**
   * @brief Gets the tester used for testing during training.
   *
   * @return Pointer to the tester.
   */
  std::shared_ptr<Testing> getTesting() const { return testing_; }

private:
  void trainFromStdin(const NetworkParameters &network_params,
                      const AppParameters &app_params);
  void trainFromFile(const NetworkParameters &network_params,
                     const AppParameters &app_params);
  RecordResult processInputLine(const NetworkParameters &network_params,
                                const AppParameters &app_params,
                                const std::string &line = "") const;
  std::shared_ptr<Network> network_ = nullptr;
  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
  std::shared_ptr<Testing> testing_ = nullptr;
};