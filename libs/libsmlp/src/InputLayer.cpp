#include "InputLayer.h"
#include "ActivationType.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>

InputLayer::InputLayer(size_t num_units, ActivationType activationType)
    : Layer(num_units, activationType, nullptr) {}

void InputLayer::forward() {
  throw std::logic_error("Invalid forward method for Input Layer.");
}

void InputLayer::forward(const std::vector<float> &input_values) {
  for (size_t i = 0; i < input_values.size(); i++) {
    neurons.at(i)->output = input_values.at(i);
  }
}
