/**
 * @file Optimizer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Optimizer interface
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
class Layer;

class Optimizer {
public:
  virtual ~Optimizer();
  // Pure virtual method that updates the weights and biases of the network
  // given the gradients of the loss with respect to the weights and biases
  virtual void UpdateWeightsAndBiases(Layer *layer, float learning_rate) = 0;
};
