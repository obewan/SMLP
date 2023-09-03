#include "AdamOptimizer.h"
#include <cmath>

AdamOptimizer::AdamOptimizer(float learning_rate, float beta1, float beta2)
    : learning_rate_(learning_rate), beta1_(beta1), beta2_(beta2) {}

void AdamOptimizer::UpdateWeightsAndBiases(Layer *layer, float learning_rate) {
  // Compute gradients for weights and biases
  std::vector<float> gradients(layer->Connections().size(), 0.0f);
  std::vector<float> bias_gradients(layer->Biases().size(), 0.0f);

  for (const Connection &connection : layer->Connections()) {
    int source_unit = connection.GetSourceUnit();
    int destination_unit = connection.GetDestinationUnit();
    // float weight = connection.GetWeight();

    float error_signal = layer->ErrorSignal(destination_unit);
    gradients[source_unit] += error_signal * layer->GetUnitValue(source_unit);
    bias_gradients[source_unit] += error_signal;
  }

  // Initialize moving averages and squared moving averages
  if (moving_averages_.empty()) {
    moving_averages_.resize(layer->Connections().size(), 0.0f);
    squared_moving_averages_.resize(layer->Connections().size(), 0.0f);
  }

  // Update moving averages and squared moving averages
  for (size_t i = 0; i < layer->Connections().size(); i++) {
    moving_averages_[i] =
        beta1_ * moving_averages_[i] + (1 - beta1_) * gradients[i];
    squared_moving_averages_[i] = beta2_ * squared_moving_averages_[i] +
                                  (1 - beta2_) * gradients[i] * gradients[i];

    float m_hat = moving_averages_[i] / (1 - std::pow(beta1_, t_));
    float v_hat = squared_moving_averages_[i] / (1 - std::pow(beta2_, t_));

    float weight_update = learning_rate * m_hat / (std::sqrt(v_hat) + epsilon_);
    layer->Connections()[i].SetWeight(layer->Connections()[i].GetWeight() -
                                      weight_update);
  }

  for (size_t i = 0; i < layer->Biases().size(); i++) {
    moving_averages_[i] =
        beta1_ * moving_averages_[i] + (1 - beta1_) * bias_gradients[i];
    squared_moving_averages_[i] =
        beta2_ * squared_moving_averages_[i] +
        (1 - beta2_) * bias_gradients[i] * bias_gradients[i];

    float m_hat = moving_averages_[i] / (1 - std::pow(beta1_, t_));
    float v_hat = squared_moving_averages_[i] / (1 - std::pow(beta2_, t_));

    float bias_update = learning_rate * m_hat / (std::sqrt(v_hat) + epsilon_);
    layer->Biases()[i] -= bias_update;
  }

  // Update t_ and check if it needs to be reset
  t_++;
  if (t_ >= reset_iterations_) {
    //  t_ = 0;
  }
}
