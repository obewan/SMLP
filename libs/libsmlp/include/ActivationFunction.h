/**
 * @file ActivationFunction.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Activation function
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "ActivationType.h"
class ActivationFunction {
public:
  ActivationFunction();

  // Constructor that takes the type of the activation function as an argument
  explicit ActivationFunction(ActivationType type);

  // Method that computes the output of the activation function given an input
  // value
  float ComputeOutput(float input);

  float ComputeDerivative(float input);

private:
  // Member variable that stores the type of the activation function
  ActivationType type_;
};
