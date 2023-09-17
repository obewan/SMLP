#include "HiddenLayer.h"
#include <algorithm>
#include <random>

HiddenLayer::HiddenLayer(size_t num_units, ActivationType activationType,
                         Layer *previous_layer)
    : Layer(num_units, activationType, previous_layer){};
