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
#include "ActivationType.h"
#include "Connection.h"
#include "Neuron.h"
#include <cstddef>
#include <vector>
class Layer {
public:
  // Constructor that takes the number of units in the layer and the activation
  // function as arguments
  Layer(size_t num_units, ActivationType activationType, Layer *previous_layer);
  virtual ~Layer();

  std::vector<Neuron *> neurons; // List of neurons in this layer
  Layer *previous_layer = nullptr;
  Layer *next_layer = nullptr;

  void ConnectTo(Layer *nextLayer);

  std::vector<Connection *> connections;

  size_t Size() const { return neurons.size(); }

  virtual void forward();

  virtual void backward(float learning_rate);

  ActivationType activation_type;
};
