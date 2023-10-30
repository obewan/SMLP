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

/**
 * @brief The OutputLayer class represents the output layer of a neural network.
 * It inherits from the Layer class and overrides its methods as necessary. This
 * layer is responsible for producing the final output of the network.
 */
class OutputLayer : public Layer {
public:
  OutputLayer() : Layer(LayerType::OutputLayer) {}
  void forwardPropagation() override {
    if (previousLayer == nullptr) {
      return;
    }
    // Implement forward propagation for output layer
    for (auto &n : neurons) {
      n.value = 0;
      for (size_t i = 0; i < previousLayer->neurons.size(); i++) {
        auto const &prev_n = previousLayer->neurons.at(i);
        n.value += prev_n.value * n.weights.at(i);
      }
      // Use sigmoid activation function
      n.value = 1.0f / (1.0f + exp(-n.value));
    }
  }

  void backwardPropagation() override {
    // No need to implement for output layer
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
