#include "Network.h"
#include "ActivationType.h"
#include "InputLayer.h"
#include "OutputLayer.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>

using namespace std;

Network::Network(size_t input_size, size_t hidden_size, size_t output_size,
                 Optimizer *optimizer, float learning_rate)
    : optimizer_(optimizer), learning_rate_(learning_rate) {

  // TODO: parameterize ActivationType
  ActivationType activationType = ActivationType::kTanh;

  // Initialize the input layer
  input_layer_ = new InputLayer(input_size, activationType);

  // Initialize the hidden layer (one hidden layer for now)
  // TODO: add more hidden layers feature
  auto hidden_layer =
      new HiddenLayer(hidden_size, activationType, input_layer_);
  hidden_layers_.push_back(hidden_layer);

  // Initialize the output layer
  output_layer_ = new OutputLayer(output_size, activationType, hidden_layer);

  // Connect the layers
  input_layer_->ConnectTo(hidden_layer);
  hidden_layer->ConnectTo(output_layer_);
}

void Network::Forward(const std::vector<float> &input_values) {
  // Set the input values of the input layer
  input_layer_->forward(input_values);
  if (monitor_ != nullptr) {
    monitor_->date();
    for (auto c : input_layer_->connections) {
      monitor_->log(c->weight);
    }
  }

  // Compute the output values of each layer in the network
  for (auto hidden_layer : hidden_layers_) {
    hidden_layer->forward();
    if (monitor_ != nullptr) {
      for (auto c : hidden_layer->connections) {
        monitor_->log(c->weight);
      }
    }
  }
  output_layer_->forward();

  if (monitor_ != nullptr) {
    for (auto v : output_layer_->GetOutputValues()) {
      monitor_->log(v);
    }
    size_t expected_size = output_layer_->GetExpectedValues().size();
    for (size_t i = 0; i < expected_size; i++) {
      auto value = output_layer_->GetExpectedValues().at(i);
      monitor_->log(value, i == expected_size - 1);
    }
  }
}

void Network::Backward() {
  // Calculate output error
  output_layer_->backward(learning_rate_);

  // Propagate error back through network
  for (auto i = (int)hidden_layers_.size() - 1; i >= 0; i--) {
    hidden_layers_[i]->backward(learning_rate_);
  }

  input_layer_->backward(learning_rate_);

  if (monitor_ != nullptr) {
    monitor_->date();
    for (auto c : input_layer_->connections) {
      monitor_->log(c->weight);
    }
    for (auto hidden_layer : hidden_layers_) {
      for (auto c : hidden_layer->connections) {
        monitor_->log(c->weight);
      }
    }
    for (auto v : output_layer_->GetOutputValues()) {
      monitor_->log(v);
    }
    size_t expected_size = output_layer_->GetExpectedValues().size();
    for (size_t i = 0; i < expected_size; i++) {
      auto value = output_layer_->GetExpectedValues().at(i);
      monitor_->log(value, i == expected_size - 1);
    }
  }
}
std::vector<float> Network::Predict(const std::vector<float> &input) const {
  // Set the input layer activations
  input_layer_->forward(input);

  // Loop over the layers in the network
  for (auto layer : hidden_layers_) {
    // Compute the activations of the neurons in the layer
    layer->forward();
  }

  output_layer_->forward();

  // Return the activations of the output layer
  return output_layer_->GetOutputValues();
}

void Network::UpdateWeightsAndBiases() {
  // Update the weights and biases using the optimizer for each hidden layer
  for (auto hidden_layer : hidden_layers_) {
    optimizer_->UpdateWeightsAndBiases(hidden_layer, learning_rate_);
  }

  // Update the weights and biases for the output layer using the optimizer
  optimizer_->UpdateWeightsAndBiases(output_layer_, learning_rate_);
}
