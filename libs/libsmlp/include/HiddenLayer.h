/**
 * @file HiddenLayer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Hidden layer
 * @date 2023-08-27
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "ActivationFunction.h"
#include "Layer.h"
#include <vector>

class HiddenLayer : public Layer {
public:
  // Constructor that takes the number of units in the hidden layer and the
  // activation function as arguments
  HiddenLayer(size_t num_units, ActivationFunction *activation_function,
              Layer *previous_layer);
};
