#include "HiddenLayer.h"
#include <algorithm>
#include <random>

HiddenLayer::HiddenLayer(size_t num_units,
                         ActivationFunction *activation_function,
                         Layer *previous_layer)
    : Layer(num_units, activation_function, previous_layer){};

void HiddenLayer::ComputeGradients() {
  // Calculate the gradients of the weights and biases using the error signals
  // and unit values
  for (size_t i = 0; i < num_units_; i++) {
    // Calculate the gradient for the bias
    gradients_[i] = error_signals_[i] *
                    activation_function_->ComputeDerivative(unit_values_[i]);

    // Calculate the gradients for the weights using the error signals and the
    // previous layer's unit values
    for (const Connection &connection : connections_) {
      if (connection.GetDestinationUnit() == i) {
        int source_unit = connection.GetSourceUnit();
        gradients_[i] +=
            error_signals_[i] *
            activation_function_->ComputeDerivative(unit_values_[i]) *
            previous_layer_->GetUnitValue(source_unit);
      }
    }
  }
}
