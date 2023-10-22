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
#include <math.h>
#include <random>
#include <vector>

class Neuron {
public:
  float value = 0.0;
  float bias = 0.0;
  float error = 0.0;
  std::vector<float> weights;

  Neuron() = default;

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
};