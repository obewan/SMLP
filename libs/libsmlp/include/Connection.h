
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
class Neuron;

class Connection {
public:
  float weight = 0.0f; // Weight of the connection
  float grad = 0.0f;   // Gradient of the weight
  float m_w = 0.0f;    // first moment estimates of the gratients
  float v_w = 0.0f;    // second moment estimates of the gratients

  Neuron *from; // The neuron from which this connection start
  Neuron *to;   // The neuron to which this connection goes

  explicit Connection(Neuron *from, Neuron *to);

private:
  // Function to generate a random float within a specified range
  float RandomFloat(float min, float max, float mean_weight,
                    float std_dev_weight) const;
};
