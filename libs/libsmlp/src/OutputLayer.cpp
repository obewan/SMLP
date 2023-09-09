#include "OutputLayer.h"
#include "Layer.h"
#include <cstddef>
#include <stdexcept>

OutputLayer::OutputLayer(size_t num_units,
                         ActivationFunction *activation_function,
                         Layer *previous_layer)
    : Layer(num_units, activation_function, previous_layer) {
  expected_values_.resize(num_units);
}

void OutputLayer::ComputeGradients() {
  // Clear the error signals and gradients for this layer
  error_signals_.clear();
  gradients_.clear();

  // Resize the error signals and gradients to the size of the layer
  error_signals_.resize(num_units_);
  gradients_.resize(num_units_);

  // Compute the gradients for each unit in the layer
  for (size_t i = 0; i < num_units_; i++) {
    // Compute the gradient of the activation function for the unit
    float gradient = activation_function_->ComputeDerivative(unit_values_[i]);

    // Compute the error signal for the unit
    error_signals_[i] = gradient * (expected_values_[i] - unit_values_[i]);

    // Compute the gradient for the unit
    gradients_[i] = error_signals_[i] * previous_layer_->GetUnitValue(i);
  }
}

void OutputLayer::SetExpectedValues(
    const std::vector<float> &expected_outputs) {
  if (expected_outputs.size() != num_units_) {
    throw std::invalid_argument("Invalid number of output values");
  }
  expected_values_ = expected_outputs;
};