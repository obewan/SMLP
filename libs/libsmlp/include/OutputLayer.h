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
#include "ActivationFunction.h"
#include "Layer.h"
#include <cstddef>
#include <ranges>
#include <vector>

class OutputLayer : public Layer {
public:
  // Constructor that takes the number of units in the output layer and the
  // activation function as arguments
  OutputLayer(size_t num_units, ActivationType activationType,
              Layer *previous_layer);

  void backward(float learning_rate) override;

  std::vector<float> GetOutputValues() {
    auto neuronOutput = [](const Neuron *n) { return n->output; };
    auto neuronOutputs = neurons | std::views::transform(neuronOutput);
    return std::vector<float>{neuronOutputs.begin(), neuronOutputs.end()};
  }

  void SetExpectedValues(const std::vector<float> &expected_values);
  std::vector<float> &GetExpectedValues() { return expected_values_; }

private:
  // vector of sample outputs, to compare with predicted outputs
  std::vector<float> expected_values_;
};
