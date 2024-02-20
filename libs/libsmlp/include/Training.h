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
#include "CommonParameters.h"
#include "DataFileParser.h"
#include "Network.h"
#include "Testing.h"
#include "exception/TrainingException.h"
#include <map>
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

namespace smlp {
enum class TrainingType { TrainingFile, TrainingSocket, TrainingStdin };

/**
 * @brief The Training class is responsible for training the neural network
 * model. It contains methods for processing training data, updating the model
 * parameters, and monitoring the progress of training.
 */
class Training {
public:
  explicit Training(TrainingType training_type);
  virtual ~Training() = default;

  const TrainingType trainingType;

  /**
   * @brief Train a network.
   *
   * @param line optional data line to use for training
   */
  virtual smlp::Result train(const std::string &line = "") = 0;

  /**
   * @brief Create a Data Parser object
   *
   */
  void createDataParser() {
    if (!dataParser_) {
      if (trainingType == TrainingType::TrainingFile) {
        dataParser_ = std::make_shared<DataFileParser>();
      } else {
        dataParser_ = std::make_shared<DataParser>();
      }
    }
  }

  /**
   * @brief Sets the data parser for training data.
   *
   * @param dataParser Pointer to the file parser.
   */
  void setDataParser(std::shared_ptr<DataParser> dataParser) {
    dataParser_ = dataParser;
  }

  /**
   * @brief Gets the data parser used for training data.
   *
   * @return Pointer to the data parser.
   */
  std::shared_ptr<DataParser> getDataParser() const { return dataParser_; }

protected:
  smlp::RecordResult processInputLine(const std::string &line = "") const;

  std::shared_ptr<DataParser> dataParser_ = nullptr;
};
} // namespace smlp