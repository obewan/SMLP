#include "Layer.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <iostream>
#include <stdexcept>
#include <vector>

Layer::Layer(size_t num_units, ActivationFunction *activation_function,
             Layer *previous_layer)
    : num_units_(num_units), activation_function_(activation_function),
      previous_layer_(previous_layer) {
  unit_values_.resize(num_units_);
  biases_.resize(num_units_);
  gradients_.resize(num_units_);
  error_signals_.resize(num_units_);
}

Layer::~Layer() = default;

size_t Layer::NumUnits() const { return num_units_; }

float Layer::GetUnitValue(size_t unit_index) const {
  return unit_values_.at(unit_index);
}

std::vector<float> Layer::GetUnitValues() const { return unit_values_; }

float Layer::GetBias(size_t index) const {
  if (index >= biases_.size()) {
    // Handle index out of range error
    throw std::out_of_range("Invalid index for GetBias");
  }
  return biases_[index];
}

void Layer::SetBias(size_t index, float value) {
  if (index >= biases_.size()) {
    // Handle index out of range error
    throw std::out_of_range("Invalid index for SetBias");
  }
  biases_[index] = value;
}

void Layer::ConnectTo(const Layer *next_layer) {
  // Create the connections between the units in the current layer and the units
  // in the specified layer
  for (size_t i = 0; i < num_units_; i++) {
    for (size_t j = 0; j < next_layer->NumUnits(); j++) {
      // Create a connection between the current unit and the unit in the
      // specified layer
      Connection connection(i, j);

      // Add the connection to the list of connections in the current layer
      connections_.push_back(connection);
    }
  }
}

void Layer::ComputeOutput() {
  // Compute the output values of this layer based on the connections and
  // weights with the previous layer's output values
  for (size_t i = 0; i < num_units_; i++) {
    float sum = 0.0;
    for (const Connection &connection : previous_layer_->Connections()) {
      if (connection.GetDestinationUnit() == i) {
        size_t source_unit = connection.GetSourceUnit();
        float weight = connection.GetWeight();
        sum += previous_layer_->GetUnitValue(source_unit) * weight;
      }
    }
    unit_values_[i] = activation_function_->ComputeOutput(sum);
  }
}

void Layer::ComputeGradients() {
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
    float error_signal = gradient * error_signals_[i];

    // Compute the gradient for the unit
    float unit_gradient = error_signal * previous_layer_->GetUnitValue(i);

    // Add the unit gradient to the gradients for the layer
    gradients_[i] += unit_gradient;

    // Add the error signal to the error signals for the layer
    error_signals_[i] += error_signal;
  }
}

void Layer::ClearGradients() {
  // Clear the gradients of the weights and biases
  std::ranges::fill(gradients_, 0.0);
  std::ranges::fill(biases_, 0.0);
}
