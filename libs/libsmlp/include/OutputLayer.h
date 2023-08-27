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
#include <vector>

class OutputLayer : public Layer {
public:
  // Constructor that takes the number of units in the output layer and the
  // activation function as arguments
  OutputLayer(int num_units, ActivationFunction *activation_function,
              Layer *previous_layer);

  // Method that accepts an array of input values and computes the output of the
  // output layer
  void ComputeOutput() override;

  void ComputeGradients() override;

  void SetOutputTargetValues(const std::vector<float> &target_outputs);

private:
  // vector of sample outputs, to compare with predicted outputs
  std::vector<float> target_outputs_;
};
