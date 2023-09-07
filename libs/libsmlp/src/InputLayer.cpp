#include "InputLayer.h"
#include <algorithm>

InputLayer::InputLayer(size_t num_units,
                       ActivationFunction *activation_function)
    : Layer(num_units, activation_function, nullptr) {}

void InputLayer::SetInput(const std::vector<float> &input_values) {
  unit_values_ = input_values;
}
