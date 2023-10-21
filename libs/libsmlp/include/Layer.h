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
class Layer {
public:
  std::vector<Neuron> neurons;
  virtual ~Layer() = default;

  virtual void forwardPropagation(Layer &prevLayer) = 0;
  virtual void backwardPropagation(Layer &nextLayer) = 0;
  virtual void updateWeights(Layer &prevLayer, float learningRate) = 0;
};