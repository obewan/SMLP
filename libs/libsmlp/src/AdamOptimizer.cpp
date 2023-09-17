#include "AdamOptimizer.h"
#include <cmath>
#include <cstddef>

/**
 * @brief Construct a new Adam Optimizer:: Adam Optimizer object
 *
 * @param learning_rate
 * @param beta1
 * @param beta2
 */

AdamOptimizer::AdamOptimizer(float learning_rate, float beta1, float beta2)
    : learning_rate_(learning_rate), beta1_(beta1), beta2_(beta2) {}

void AdamOptimizer::UpdateWeightsAndBiases(Layer *layer, float learning_rate) {
  // // Initialize hyperparameters if not already done
  // if (beta1t_ == 0.0f && beta2t_ == 0.0f) {
  //   beta1t_ = beta1_;
  //   beta2t_ = beta2_;
  // }
  // for (auto neuron : layer->neurons) {
  //   for (auto connection : neuron->outgoing_connections) {
  //     // Calculate the gradients
  //     float grad_w = connection->grad;

  //     // Update biased first and second moment estimates for weights
  //     connection->m_w = beta1_ * connection->m_w + (1.0f - beta1_) * grad_w;
  //     connection->v_w =
  //         beta2_ * connection->v_w + (1.0f - beta2_) * grad_w * grad_w;

  //     // Compute bias-corrected first and second moment estimates for weights
  //     float m_hat_w = connection->m_w / (1.0f - beta1t_);
  //     float v_hat_w = connection->v_w / (1.0f - beta2t_);

  //     // Update weights
  //     connection->weight -=
  //         learning_rate * m_hat_w / (sqrt(v_hat_w) + epsilon_);
  //   }
  // }

  // // Update the biased moment estimates for next time step
  // beta1t_ *= beta1_;
  // beta2t_ *= beta2_;
}
