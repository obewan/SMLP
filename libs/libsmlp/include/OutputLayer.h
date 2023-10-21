/**
 * @file OutputLayer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Output layer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Layer.h"
#include "Neuron.h"
#include <ranges>
#include <vector>

// note:
// n.value = 1 / (1 + exp(-n.value));
// is optimized: see
// https://stackoverflow.com/questions/10732027/fast-sigmoid-algorithm

class OutputLayer : public Layer {
public:
  void forwardPropagation(Layer &prevLayer) override {
    // Implement forward propagation for output layer
    for (auto &n : neurons) {
      n.value = 0;
      for (size_t i = 0; i < prevLayer.neurons.size(); i++) {
        auto const &prev_n = prevLayer.neurons.at(i);
        n.value += prev_n.value * n.weights.at(i);
      }
      // Use sigmoid activation function
      n.value = 1.0f / (1.0f + exp(-n.value));
    }
  }

  void backwardPropagation(Layer &nextLayer) override {
    // No need to implement for output layer
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

  void computeErrors(std::vector<float> const &expectedValues) {
    if (expectedValues.size() != neurons.size()) {
      throw std::invalid_argument("Invalid expected output size");
    }
    for (size_t i = 0; i < neurons.size(); i++) {
      neurons[i].error = neurons[i].value - expectedValues[i];
    }
  }

  std::vector<float> getOutputValues() {
    auto neuronOutput = [](const Neuron &n) { return n.value; };
    auto neuronOutputs = neurons | std::views::transform(neuronOutput);
    return std::vector<float>{neuronOutputs.begin(), neuronOutputs.end()};
  }
};
