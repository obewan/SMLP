#include "HiddenLayer.h"
#include <algorithm>
#include <random>

HiddenLayer::HiddenLayer(size_t num_units,
                         ActivationFunction *activation_function,
                         Layer *previous_layer)
    : Layer(num_units, activation_function, previous_layer){};
