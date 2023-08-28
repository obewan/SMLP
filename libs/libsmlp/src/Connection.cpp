#include "Connection.h"
#include <random>

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