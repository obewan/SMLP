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
#include <cstddef>

class HiddenLayer : public Layer {
public:
  void forwardPropagation(Layer &prevLayer) override {
    // Implement forward propagation for hidden layer
    for (auto &n : neurons) {
      n.value = 0.0;
      for (size_t i = 0; i < prevLayer.neurons.size(); i++) {
        auto const &prev_n = prevLayer.neurons.at(i);
        n.value += prev_n.value * n.weights.at(i);
      }
      // Use sigmoid activation function
      n.value = 1.0f / (1.0f + exp(-n.value));
    }
  }

  void backwardPropagation(Layer &nextLayer) override {
    // Implement backward propagation for hidden layer
    for (size_t i = 0; i < neurons.size(); ++i) {
      neurons[i].error = 0.0;
      for (Neuron &n : nextLayer.neurons)
        neurons[i].error += n.weights[i] * n.error;
      neurons[i].error *= neurons[i].value * (1 - neurons[i].value);
    }
  }

  void updateWeights(Layer &prevLayer, float learningRate) override {
    for (Neuron &n : neurons) {
      for (size_t j = 0; j < n.weights.size(); ++j) {
        // Gradient descent
        float dE_dw = prevLayer.neurons[j].value * n.error;
        // Update weights
        n.weights[j] -= learningRate * dE_dw;
      }
    }
  }
};
