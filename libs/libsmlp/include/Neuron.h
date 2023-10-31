/**
 * @file Neuron.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Neuron class
 * @date 2023-09-13
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "ActivationFunctions.h"
#include <functional>
#include <math.h>
#include <random>
#include <vector>

/**
 * @brief The Neuron class represents a neuron in a neural network. It contains
 * a value, bias, error, and a vector of weights. It also has methods for
 * initializing weights.
 */
class Neuron {
public:
  // The value of the neuron
  float value = 0.0;

  // The bias of the neuron
  float bias = 0.0;

  // The error of the neuron
  float error = 0.0;

  // The weights of the neuron
  std::vector<float> weights;

  // Default constructor
  Neuron() = default;

  /**
   * @brief Initializes the weights of the neuron to a given size. The weights
   * are randomized to break symmetry.
   *
   * @param new_size The new size of the weights vector.
   */
  void initWeights(size_t new_size) {
    weights.resize(new_size);

    // randomize weights to break symmetry
    std::random_device rd;
    for (auto &w : weights) {
      std::mt19937 gen(rd());
      std::normal_distribution<float> dist(0.1f, 0.01f);
      float distg = dist(gen);
      w = std::max(0.0f, std::min(0.1f, distg));
    }
  }

  std::function<float(float)> activationFunction;
  std::function<float(float)> activationFunctionDerivative;
};