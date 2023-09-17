/**
 * @file Network.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Neural network class that contains layers and main methods
 * @date 2023-08-26
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */

#pragma once
#include "HiddenLayer.h"
#include "InputLayer.h"
#include "Layer.h"
#include "Monitor.h"
#include "Optimizer.h"
#include "OutputLayer.h"
#include <cstddef>
#include <vector>

/**
 The methods for getting and setting weights and biases are also necessary
 for implementing backpropagation during training.
 The addition of InitializeWeights() will also be useful
 for initializing the weights in the network before training.

 The Forward() method takes a std::vector<float> input and returns a float*
output. It's important to make sure that the memory for the output is allocated
 before passing it to the ComputeOutput() method of the output layer.

The Backward() and ClearGradient() methods will be used during backpropagation
to compute the gradients of the weights and biases in the network.

Finally, the Predict() method takes a std::vector<float> input and returns a
std::vector<float> output. This method will be useful for using the trained
network to make predictions on new input data.
*/

class Network {
public:
  // Constructor that takes the sizes of the input, hidden, and output layers as
  // arguments
  Network(size_t input_size, size_t hidden_size, size_t output_size,
          Optimizer *optimizer, float learning_rate);

  // Method that returns a pointer to the specified layer in the network
  InputLayer *GetInputLayer() { return input_layer_; };
  std::vector<HiddenLayer *> &GetHiddenLayers() { return hidden_layers_; }
  HiddenLayer *GetHiddenLayer(size_t index) {
    return hidden_layers_.at(index);
  };
  OutputLayer *GetOutputLayer() { return output_layer_; };

  void SetMonitor(Monitor *monitor) { monitor_ = monitor; }
  Monitor *GetMonitor() { return monitor_; }

  size_t GetInputSize() const { return input_layer_->Size(); }
  size_t GetOutputSize() const { return output_layer_->Size(); }

  void Forward(const std::vector<float> &input_values);
  void Backward();
  std::vector<float> Predict(const std::vector<float> &input) const;

  void UpdateWeightsAndBiases();

private:
  InputLayer *input_layer_;
  std::vector<HiddenLayer *> hidden_layers_;
  OutputLayer *output_layer_;
  Optimizer *optimizer_;
  Monitor *monitor_ = nullptr;
  float learning_rate_;
};
