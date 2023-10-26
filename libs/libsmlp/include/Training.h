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
#include "FileParser.h"
#include "Network.h"
#include "Testing.h"
#include "exception/TrainingException.h"
#include <bits/types/FILE.h>
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

class Training {
public:
  Training() : network_(nullptr), fileParser_("") {}
  // Constructor that takes a pointer to the network and the file path to
  // training data as arguments
  Training(Network *network, const std::string &file_path)
      : network_(network), fileParser_(file_path) {}

  void train(const Parameters &params);
  void trainAndTest(const Parameters &params);

  void setNetwork(Network *network) { network_ = network; }
  Network *getNetwork() { return network_; }

  void setFileParserPath(const std::string &file_path) {
    fileParser_.path = file_path;
  }
  FileParser &getFileParser() { return fileParser_; }

private:
  Network *network_;
  FileParser fileParser_;
};