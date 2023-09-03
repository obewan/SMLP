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
#include <vector>

class InputLayer : public Layer {
public:
  InputLayer(size_t num_units, ActivationFunction *activation_function);

  void ComputeOutput() override;

  // Method that accepts an array of input values and stores them in the input
  // layer
  void SetInput(const std::vector<float> &input_values);
};
