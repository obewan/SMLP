#include "Layer.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cstddef>
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

float Layer::GetWeight(size_t index) {
  if (index >= connections_.size()) {
    throw std::out_of_range("Invalid index for GetWeight");
  }
  return connections_.at(index).GetWeight();
}

void Layer::SetWeight(size_t index, float value) {
  if (index >= connections_.size()) {
    throw std::out_of_range("Invalid index for SetWeight");
  }
  connections_.at(index).SetWeight(value);
}

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

void Layer::ConnectTo(Layer *next_layer) {
  next_layer_ = next_layer;

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
  if (previous_layer_ == nullptr) {
    return;
  }
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
  if (next_layer_ == nullptr) {
    return;
  }
  // Calculate the gradients of the weights and biases using the error signals
  // and unit values
  for (size_t i = 0; i < num_units_; i++) {
    gradients_[i] = 0.0f;

    // Calculate the gradients for the weights using the error signals and the
    // previous layer's unit values
    for (const Connection &connection : connections_) {
      if (connection.GetSourceUnit() == i) {
        auto destination_index = connection.GetDestinationUnit();
        auto destination_error_signal =
            next_layer_->ErrorSignal(destination_index);

        // Calculate the gradient for the bias
        gradients_[i] +=
            destination_error_signal *
            activation_function_->ComputeDerivative(unit_values_[i]) *
            previous_layer_->GetUnitValue(i);

        // Calculate the error_signal of this layer
        error_signals_[i] +=
            destination_error_signal *
            activation_function_->ComputeDerivative(unit_values_[i]) *
            connection.GetWeight();
      }
    }
  }
}

void Layer::ClearGradients() {
  // Clear the gradients of the weights and biases
  std::ranges::fill(gradients_, 0.0);
  std::ranges::fill(biases_, 0.0);
}
