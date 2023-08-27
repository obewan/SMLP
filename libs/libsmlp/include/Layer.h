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
#include "ActivationFunction.h"
#include "Connection.h"
#include <vector>
class Layer {
public:
  // Constructor that takes the number of units in the layer and the activation
  // function as arguments
  Layer(size_t num_units, ActivationFunction *activation_function,
        Layer *previous_layer);
  virtual ~Layer();

  // Pure virtual method that computes the output of the layer given an array of
  // input values
  virtual void ComputeOutput() = 0;

  virtual void ComputeGradients();

  // Accessor method for the error signals (gradients) of the layer
  float ErrorSignal(size_t pos) { return error_signals_.at(pos); }

  void SetErrorSignal(size_t pos, float value) {
    error_signals_.at(pos) = value;
  };

  // non const accessor (for update)
  std::vector<float> &Biases() { return biases_; }

  std::vector<float> &Gradients() { return gradients_; }

  // non const accessor (for update)
  std::vector<Connection> &Connections() { return connections_; }

  // Method that returns the number of units in the layer
  size_t NumUnits() const;

  // Method that returns the value of a specific unit in the layer
  float GetUnitValue(int unit_index) const;

  std::vector<float> GetUnitValues() const;

  float GetBias(int index) const;
  void SetBias(int index, float value);

  void ConnectTo(const Layer *next_layer);
  void ConnectTo(const Layer *next_layer, const std::vector<float> &weights);

  void ClearGradients();

protected:
  // Member variable that stores the number of units in the layer
  size_t num_units_;

  // Pointer to the activation function used by the layer
  ActivationFunction *activation_function_;

  // Array that stores the biases of the units in the layer
  std::vector<float> biases_;

  // Array that stores the values of the units in the layer
  // store the output values of the units in a layer during the forward
  // propagation process.
  std::vector<float> unit_values_;

  std::vector<Connection> connections_;

  // Pointer to the previous layer
  Layer *previous_layer_;

  virtual void InitializeWeights();

  // Array that stores the error signals for each unit in the layer
  std::vector<float> error_signals_;

  // Array that stores the gradients for each weight in the layer
  std::vector<float> gradients_;
};