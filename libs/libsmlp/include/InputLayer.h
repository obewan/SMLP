/**
 * @file InputLayer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Input layer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Layer.h"

class InputLayer : public Layer {
public:
  void forwardPropagation(Layer &prevLayer) override {
    // No need to implement for input layer
  }

  void backwardPropagation(Layer &nextLayer) override {
    // Implement backward propagation for input layer
    for (size_t i = 0; i < neurons.size(); ++i) {
      neurons[i].error = 0.0;
      for (Neuron &n : nextLayer.neurons)
        neurons[i].error += n.weights[i] * n.error;
      neurons[i].error *= neurons[i].value * (1 - neurons[i].value);
    }
  }

  void updateWeights(Layer &prevLayer, float learningRate) override {
    // No need to implement for input layer (no weights of input layer)
  }

  void setInputValues(const std::vector<float> &inputValues) {
    if (inputValues.size() != neurons.size()) {
      throw std::invalid_argument("Invalid number of input values");
    }
    for (size_t i = 0; i < neurons.size(); i++) {
      neurons.at(i).value = inputValues.at(i);
    }
  }
};
