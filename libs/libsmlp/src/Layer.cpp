#include "Layer.h"
#include "Connection.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cstddef>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

Layer::Layer(size_t size, ActivationType activationType, Layer *previousLayer)
    : previous_layer(previousLayer), activation_type(activationType) {
  for (size_t i = 0; i < size; i++) {
    neurons.push_back(new Neuron(activationType, (int)i + 1));
  }
}

Layer::~Layer() = default;

void Layer::ConnectTo(Layer *nextLayer) {
  next_layer = nextLayer;
  // Create connections from each neuron in this layer to each neuron in the
  // next layer
  for (Neuron *from : neurons) {
    for (Neuron *to : nextLayer->neurons) {
      connections.push_back(new Connection(from, to));
    }
  }
};

void Layer::forward() {
  // Implement forward propagation here
  // For example, you might pass each input through its corresponding neuron
  // and return the results
  std::vector<Connection *> incoming_connexions;
  for (auto neuron : neurons) {
    for (auto connection : previous_layer->connections) {
      if (connection->to == neuron) {
        incoming_connexions.push_back(connection);
      }
    }
    neuron->forward(incoming_connexions);
    incoming_connexions.clear();
  }
}

void Layer::backward(float learning_rate) {
  // Calculate errors for each neuron in this layer
  std::map<Neuron *, float> errors;
  for (auto connection : connections) {
    connection->grad = connection->to->error;
    connection->weight -= learning_rate * connection->grad;
    errors[connection->to] += connection->to->error * connection->weight;
  }
  for (auto [neuron, error] : errors) {
    neuron->computeError(error);
  }
}