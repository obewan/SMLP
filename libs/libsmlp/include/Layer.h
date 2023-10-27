/**
 * @file Layer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Abstract layer class
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Neuron.h"

/**
 * @brief The Layer class represents a layer in a neural network. It contains a
 * vector of Neurons and has methods for forward propagation, backward
 * propagation, and updating weights.
 */
class Layer {
public:
  // Vector of Neurons in this layer
  std::vector<Neuron> neurons;

  // Virtual destructor
  virtual ~Layer() = default;

  /**
   * @brief Performs forward propagation using the previous layer.
   *
   * @param prevLayer The previous layer in the network.
   */
  virtual void forwardPropagation(Layer &prevLayer) = 0;

  /**
   * @brief Performs backward propagation using the next layer.
   *
   * @param nextLayer The next layer in the network.
   */
  virtual void backwardPropagation(Layer &nextLayer) = 0;

  /**
   * @brief Updates the weights of the neurons in this layer using the previous
   * layer and a learning rate.
   *
   * @param prevLayer The previous layer in the network.
   * @param learningRate The learning rate to use when updating weights.
   */
  virtual void updateWeights(Layer &prevLayer, float learningRate) = 0;
};