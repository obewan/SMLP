/**
 * @file AdamOptimizer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Adam optimizer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Layer.h"
#include "Network.h"
#include "Optimizer.h"

/**
 * @brief
 * The role of the Adam optimizer (or any optimizer) is to take those gradients
 * and apply an optimization algorithm to update the weights and biases of your
 * network. The Adam optimizer doesn't calculate gradients itself; it uses the
 * gradients you computed in the previous step. It applies techniques like
 * adaptive learning rates, momentum, and more to perform weight updates in a
 * way that helps the network converge faster.
 */
class AdamOptimizer : public Optimizer {
public:
  // Constructor that takes the learning rate and the decay rates for the moving
  // averages as arguments
  AdamOptimizer(float learning_rate = 1e-3f, float beta1 = 0.9f,
                float beta2 = 0.99f);

  // Method that updates the weights and biases of the network given the
  // gradients of the loss with respect to the weights and biases
  void UpdateWeightsAndBiases(Layer *layer, float learning_rate) override;

  float LearningRate() const { return learning_rate_; }
  float Beta1() const { return beta1_; }
  float Beta2() const { return beta2_; }

private:
  // Member variables that store the learning rate and the decay rates for the
  // moving averages
  float learning_rate_;
  float beta1_;
  float beta2_;

  // Member variable that stores the time step
  size_t t_ = 1;

  // Member variable that specifies the number of iterations after which t_ will
  // be reset to zero
  size_t reset_iterations_ = 1000;

  // Member variable that stores the small constant to prevent division by zero
  const float epsilon_ = 1e-8f;

  // Member variable that stores the moving averages of the gradients of the
  // weights and biases
  std::vector<float> moving_averages_;

  // Member variable that stores the squared moving averages of the gradients of
  // the weights and biases
  std::vector<float> squared_moving_averages_;
};