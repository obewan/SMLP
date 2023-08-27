#include "ActivationFunction.h"
#include <math.h>

ActivationFunction::ActivationFunction()
{
    type_ = ActivationType::kSigmoid;
}

ActivationFunction::ActivationFunction(ActivationType type)
{
    type_ = type;
}

float ActivationFunction::ComputeOutput(float input)
{
    if (type_ == kSigmoid)
    {
        // Return the result of applying the sigmoid function to the input value
        return 1.0f / (1.0f + exp(-input));
    }
    return 0.0f;
}

float ActivationFunction::ComputeDerivative(float input)
{
    float output = ComputeOutput(input);
    if (type_ == kSigmoid)
    {
        // Compute the derivative of the sigmoid function
        return output * (1.0f - output);
    }
    return 0.0f;
}