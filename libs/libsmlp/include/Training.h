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
#include <SimpleLogger.h>
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

enum class TrainingType { TrainingFile, TrainingSocket, TrainingStdin };

const std::map<std::string, TrainingType, std::less<>> training_map{
    {"TrainingFile", TrainingType::TrainingFile},
    {"TrainingSocket", TrainingType::TrainingSocket},
    {"TrainingStdin", TrainingType::TrainingStdin}};

/**
 * @brief The Training class is responsible for training the neural network
 * model. It contains methods for processing training data, updating the model
 * parameters, and monitoring the progress of training.
 */
class Training {
public:
  explicit Training(TrainingType training_type);
  virtual ~Training() = default;

  TrainingType trainingType;

  const std::string UndefinedTraining = "UndefinedTraining";

  /**
   * @brief Train a network.
   *
   * @param line optional data line to use for training
   */
  virtual void train(const std::string &line = "") = 0;

  /**
   * @brief Create a File Parser object
   *
   */
  void createFileParser() {
    if (!fileParser_) {
      fileParser_ = std::make_shared<DataFileParser>();
    }
  }

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

  std::string trainingTypeStr() const {
    for (const auto &[key, mTrainingType] : training_map) {
      if (mTrainingType == trainingType) {
        return key;
      }
    }
    return UndefinedTraining;
  }

protected:
  RecordResult processInputLine(const std::string &line = "") const;

  const SimpleLogger &logger_ = SimpleLogger::getInstance();

  std::shared_ptr<DataFileParser> fileParser_ = nullptr;
};
