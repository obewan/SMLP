#include "Connection.h"
#include "Neuron.h"
#include <random>

Connection::Connection(Neuron *from, Neuron *to) : from(from), to(to) {
  weight = RandomFloat(0.0f, 1.0f, 0.1f, 0.01f);
}

float Connection::RandomFloat(float min, float max, float mean_weight,
                              float std_dev_weight) const {
  // Initialize the weight using a normal distribution
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<float> dist(mean_weight, std_dev_weight);
  float distg = dist(gen);

  // Truncate the weight to ensure it falls within the desired range
  return std::max(min, std::min(max, distg));
}