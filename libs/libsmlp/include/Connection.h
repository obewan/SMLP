
/**
 * @file Connection.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Connection between two neurons
 * @date 2023-08-26
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
class Connection {
public:
  Connection(int source_unit, int destination_unit, float weight,
             float min_weight = 0.0f, float max_weight = 1.0f,
             float mean_weight = 0.5f, float std_dev_weight = 0.2f)
      : source_unit_(source_unit), destination_unit_(destination_unit),
        weight_(weight) {
    weight_ = RandomFloat(min_weight, max_weight, mean_weight, std_dev_weight);
  }

  // Source unit in the layer n
  int GetSourceUnit() const { return source_unit_; }

  // Destination unit in the layer n+1
  int GetDestinationUnit() const { return destination_unit_; }

  float GetWeight() const { return weight_; }
  void SetWeight(float weight) { weight_ = weight; }

private:
  int source_unit_;
  int destination_unit_;
  float weight_;

  // Function to generate a random float within a specified range
  float RandomFloat(float min, float max, float mean_weight,
                    float std_dev_weight) const;
};
