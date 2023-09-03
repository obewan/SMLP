#include "ActivationFunction.h"
#include "ActivationType.h"
#include <math.h>

ActivationFunction::ActivationFunction() { type_ = ActivationType::kSigmoid; }

ActivationFunction::ActivationFunction(ActivationType type) : type_(type) {}

float ActivationFunction::ComputeOutput(float input) {
  switch (type_) {
    // Return the result of applying the sigmoid function to the input value
  case ActivationType::kSigmoid:
    return 1.0f / (1.0f + exp(-input));
    // Return the result of applying the hyperbolic tangent function to the
    // input value
  case ActivationType::kTanh:
    return tanh(input);
  default:
    return 0.0f;
  }
}

float ActivationFunction::ComputeDerivative(float input) {
  float output = ComputeOutput(input);
  switch (type_) {
    // Compute the derivative of the sigmoid function
  case ActivationType::kSigmoid:
    return output * (1.0f - output);
  // Calculate the derivative of tanh
  case ActivationType::kTanh:
    return 1.0f - output * output;
  default:
    return 0.0f;
  }
}