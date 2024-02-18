/**
 * @file HiddenLayer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Hidden layer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Layer.h"

namespace smlp {
/**
 * @brief The HiddenLayer class represents a hidden layer in a neural network.
 * It inherits from the Layer class and overrides its methods as necessary.
 * Hidden layers are responsible for processing inputs received from the input
 * layer and passing the result to the output layer or the next hidden layer.
 */
class HiddenLayer : public Layer {
public:
  HiddenLayer() : Layer(LayerType::HiddenLayer) {}

  void forwardPropagation() override {
    if (previousLayer == nullptr) {
      return;
    }
    // Implement forward propagation for hidden layer
    for (auto &n : neurons) {
      n.value = 0.0;
      for (size_t i = 0; i < previousLayer->neurons.size(); i++) {
        auto const &prev_n = previousLayer->neurons.at(i);
        n.value += prev_n.value * n.weights.at(i);
      }
      // Use activation function
      n.value = n.activationFunction(n.value);
    }
  }

  void backwardPropagation() override {
    if (nextLayer == nullptr) {
      return;
    }

    // Implement backward propagation for hidden layer
    for (size_t i = 0; i < neurons.size(); ++i) {
      neurons[i].error = 0.0;
      for (Neuron &n : nextLayer->neurons)
        neurons[i].error += n.weights[i] * n.error;
      // Use the derivative of the activation function
      neurons[i].error *=
          neurons[i].activationFunctionDerivative(neurons[i].value);
    }
  }

  void updateWeights(float learningRate) override {
    if (previousLayer == nullptr) {
      return;
    }
    for (Neuron &n : neurons) {
      for (size_t j = 0; j < n.weights.size(); ++j) {
        // Gradient descent
        float dE_dw = previousLayer->neurons[j].value * n.error;
        // Update weights
        n.weights[j] -= learningRate * dE_dw;
      }
    }
  }
};
} // namespace smlp