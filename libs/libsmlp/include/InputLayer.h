/**
 * @file InputLayer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Input layer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Layer.h"
#include "SimpleLang.h"
#include <stdexcept>

namespace smlp {
/**
 * @brief The InputLayer class represents the input layer of a neural network.
 * It inherits from the Layer class and overrides its methods as necessary. This
 * layer is responsible for receiving input from external sources.
 */
class InputLayer : public Layer {
public:
  InputLayer() : Layer(LayerType::InputLayer) {}
  void forwardPropagation() override {
    // No need to implement for input layer
  }

  void backwardPropagation() override {
    // No need to implement for input layer (no weights of input layer)
  }

  void updateWeights(float learningRate) override {
    // No need to implement for input layer (no weights of input layer)
  }

  void setInputValues(const std::vector<float> &inputValues) {
    if (inputValues.size() != neurons.size()) {
      throw std::invalid_argument(SimpleLang::Error(Error::InvalidTotalInput));
    }
    for (size_t i = 0; i < neurons.size(); i++) {
      neurons.at(i).value = inputValues.at(i);
    }
  }
};
} // namespace smlp