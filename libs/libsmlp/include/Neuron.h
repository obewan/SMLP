/**
 * @file Neuron.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Neuron class
 * @date 2023-09-13
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "ActivationType.h"
#include "Connection.h"
#include <math.h>
#include <stdexcept>
#include <vector>

class Neuron {
public:
  explicit Neuron(ActivationType activationType, int id, float weight = 0.f)
      : activation_type(activationType), id(id), w(weight) {}

  ActivationType activation_type;

  int id = 0;

  float w = 0.f; // Weight
  float b = 0.f; // Bias

  float grad_w = 0.f; // Gradient of the weight
  float grad_b = 0.f; // Gradient of the bias

  float m_w = 0.f; // First moment estimate of the weight gradient
  float v_w = 0.f; // Second moment estimate of the weight gradient

  float m_b = 0.f; // First moment estimate of the bias gradient
  float v_b = 0.f; // Second moment estimate of the bias gradient

  float input = 0.f;
  float output = 0.f;
  float error = 0.f;

  /**
   * @brief The forward method in the Neuron class is responsible for performing
   * the forward propagation for a single neuron. This involves taking an input,
   * multiplying it by the neuron’s weight, adding the bias, and then applying
   * an activation function.
   *
   * @param incoming_connections
   * @return
   */
  void forward(const std::vector<Connection *> &incoming_connections) {
    output = 0;
    for (auto incoming_connection : incoming_connections) {
      output +=
          incoming_connection->weight * incoming_connection->from->output +
          b; // Apply weight and bias
    }
    output = activation(output);
  }

  /**
   * @brief The backward method in the Neuron class is responsible for
   * performing the backward propagation for a single neuron. This involves
   * calculating the gradient of the error with respect to the neuron’s
   * parameters (weights and biases), and then updating the parameters using
   * this gradient.
   *
   * @param p_error
   * @param learning_rate
   */
  void backward(float p_error, float learning_rate) {
    // Calculate the gradient of the error with respect to the weight and bias
    grad_w = p_error * input; // For simplicity, assuming 'input' is stored from
                              // the forward pass
    grad_b = p_error;

    // Update the weight and bias
    w -= learning_rate * grad_w;
    b -= learning_rate * grad_b;
  }

  /**
   * @brief calculate the activation
   *
   * @param value
   * @return float
   */
  float activation(float value) const {
    switch (activation_type) {
    case ActivationType::kSigmoid:
      return 1.0f / (1.0f + exp(-value));
    case ActivationType::kTanh:
      return tanh(value);
    default:
      throw std::logic_error("Activation type not implemented");
    }
  }

  void computeError(float factor) { error = factor * derivative(output); }

  /**
   * @brief calculate the derivative
   *
   * @param value
   * @return float
   */
  float derivative(float value) const {
    switch (activation_type) {
    case ActivationType::kSigmoid:
      return value * (1.0f - value);
    case ActivationType::kTanh:
      return 1.0f - value * value;
    default:
      throw std::logic_error("Activation type not implemented");
    }
  }
};
