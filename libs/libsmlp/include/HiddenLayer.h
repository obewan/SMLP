/**
 * @file HiddenLayer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Hidden layer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Layer.h"

/**
About Sigmoid activation function:
In a Multilayer Perceptron (MLP), the sigmoid function is commonly used as the
activation function during the forward propagation step. The reason for this is
that the sigmoid function maps any input value into a range between 0 and 1,
which can be useful for outputting probabilities, among other things1.

During backpropagation, which is the process of updating the weights in the
network, we use the derivative of the sigmoid function. The derivative of the
sigmoid function has some nice properties that make it particularly useful for
backpropagation2.

One of these properties is that it can be expressed in terms of the output of
the sigmoid function itself: if σ(x) is the sigmoid function, then its
derivative σ'(x) can be computed as σ(x) * (1 - σ(x)). This means that once
you’ve computed the forward pass, you can easily compute the derivative without
having to go through the entire computation again2.

The derivative of the sigmoid function is used to compute the gradient of the
loss function with respect to each weight in the network. This gradient tells us
how much we need to adjust each weight to minimize the loss2.

In summary, we use the sigmoid function in forward propagation because it’s a
good activation function that maps inputs to a range between 0 and 1, and we use
its derivative in backpropagation because it helps us compute gradients
efficiently1
*/

/**
TODO: add sigmoid alternatives:
 - Tanh Function (Hyperbolic Tangent): This function is similar to the sigmoid
 function but maps the input to a range between -1 and 1. It is often used in
 the hidden layers of a neural network1.
 - ReLU Function (Rectified Linear Unit): This function outputs the input
 directly if it’s positive; otherwise, it outputs zero. It has become very
 popular in recent years because it helps to alleviate the vanishing gradient
 problem1.
 - Leaky ReLU Function: This is a variant of ReLU that allows small negative
 values when the input is less than zero. It can help to prevent dead neurons
 and offer a more balanced approach1.
 - Parametric ReLU Function: This is another variant of ReLU that allows the
 negative slope to be learned during training rather than being fixed
 beforehand1.
 - Exponential Linear Units (ELUs) Function: This function tends to converge
 cost to zero faster and produce more accurate results. Different from ReLU,
 ELU has a non-zero gradient for negative input1.
 - Generalized Riccati Activation (GRA): This is an alternative generalized
 adaptive solution to the fixed sigmoid, which has been shown to yield higher
 accuracy than any other combination of activation functions in certain
 cases2.*/

/**
 * @brief The HiddenLayer class represents a hidden layer in a neural network.
 * It inherits from the Layer class and overrides its methods as necessary.
 * Hidden layers are responsible for processing inputs received from the input
 * layer and passing the result to the output layer or the next hidden layer.
 */
class HiddenLayer : public Layer {
public:
  void forwardPropagation() override {
    if (previousLayer == nullptr) {
      return;
    }
    // Implement forward propagation for hidden layer
    for (auto &n : neurons) {
      n.value = 0.0;
      for (size_t i = 0; i < previousLayer->neurons.size(); i++) {
        auto const &prev_n = previousLayer->neurons.at(i);
        n.value += prev_n.value * n.weights.at(i);
      }
      // Use sigmoid activation function
      n.value = 1.0f / (1.0f + exp(-n.value));
    }
  }

  void backwardPropagation() override {
    if (nextLayer == nullptr) {
      return;
    }

    // Implement backward propagation for hidden layer
    for (size_t i = 0; i < neurons.size(); ++i) {
      neurons[i].error = 0.0;
      for (Neuron &n : nextLayer->neurons)
        neurons[i].error += n.weights[i] * n.error;
      // Use the derivative of the sigmoid function
      neurons[i].error *= neurons[i].value * (1 - neurons[i].value);
    }
  }

  void updateWeights(float learningRate) override {
    if (previousLayer == nullptr) {
      return;
    }
    for (Neuron &n : neurons) {
      for (size_t j = 0; j < n.weights.size(); ++j) {
        // Gradient descent
        float dE_dw = previousLayer->neurons[j].value * n.error;
        // Update weights
        n.weights[j] -= learningRate * dE_dw;
      }
    }
  }
};
