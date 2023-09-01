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
#include "FileParser.h"
#include "Network.h"
#include "Optimizer.h"

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

class Training : public FileParser {
public:
  // Constructor that takes a pointer to the network and the file path to
  // training data as arguments
  Training(Network *network, const std::string &file_path, Optimizer *optimizer)
      : FileParser(file_path), network_(network), optimizer_(optimizer) {}

  /**
   * @brief Method that trains the network using online training
   *
   * @param num_epochs number of file data retraining
   * @param ouput_at_end if true the output value must be at end of record, else
   * at start.
   * @param from_line start training from this line, must be lesser than
   * total lines.
   * @param to_line end training at this line (including), must be greater than
   * from_line or equal to 0 for end of file. Useful to keep other lines for
   * testing.
   * @return bool success
   */
  bool Train(size_t num_epochs, bool output_at_end = true, size_t from_line = 0,
             size_t to_line = 0);

private:
  Network *network_;
  Optimizer *optimizer_;

  bool ProcessEpoch(size_t from_line, size_t to_line, bool output_at_end);
};