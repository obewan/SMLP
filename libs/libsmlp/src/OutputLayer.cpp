#include "OutputLayer.h"
#include "Layer.h"
#include <cstddef>
#include <stdexcept>

OutputLayer::OutputLayer(size_t num_units, ActivationType activationType,
                         Layer *previous_layer)
    : Layer(num_units, activationType, previous_layer) {
  expected_values_.resize(num_units);
}

void OutputLayer::backward(float learning_rate) {
  for (size_t i = 0; i < neurons.size(); i++) {
    float output =
        neurons[i]->output; // Assuming 'output' is stored from the forward pass
    float target = GetExpectedValues()[i];
    neurons[i]->computeError(output - target);
  }
}

void OutputLayer::SetExpectedValues(
    const std::vector<float> &expected_outputs) {
  if (expected_outputs.size() != Size()) {
    throw std::invalid_argument("Invalid number of output values");
  }
  expected_values_ = expected_outputs;
};