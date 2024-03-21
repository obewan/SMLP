/**
 * @file Network.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Neural network class that contains layers and main methods
 * @date 2023-08-26
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */

#pragma once
#include "ActivationFunctions.h"
#include "Common.h"
#include "HiddenLayer.h"
#include "InputLayer.h"
#include "OutputLayer.h"
#include "SimpleLang.h"
#include "exception/NetworkException.h"

namespace smlp {
/**
 * @brief The Network class represents a neural network model. It contains an
 * input layer, an output layer, and a vector of hidden layers. It also has a
 * learning rate and methods for forward propagation, backward propagation, and
 * updating weights.
 */
class Network {
public:
  // Rule of Five:
  Network(const Network &other) = delete;            // Copy constructor
  Network &operator=(const Network &other) = delete; // Copy assignment operator
  Network(Network &&other) = default;                // Move constructor
  Network &operator=(Network &&other) = default;     // Move assignment operator
  ~Network() {
    for (auto layer : layers) {
      delete layer;
    }
  }

  /**
   * @brief Construct a new Network object
   * Default constructor used for deserialization.
   */
  Network() = default;

  std::vector<Layer *> layers;

  /**
   * @brief Performs forward propagation on the network using the given input
   * values.
   *
   * @param inputValues The input values for forward propagation.
   * @return A vector of output values from the output layer after forward
   * propagation.
   */
  std::vector<float> forwardPropagation(const std::vector<float> &inputValues);

  /**
   * @brief Performs backward propagation on the network using the given
   * expected values.
   *
   * @param expectedValues The expected values for backward propagation.
   */
  void backwardPropagation(const std::vector<float> &expectedValues);

  /**
   * @brief Updates the weights of the neurons in the network using the learning
   * rate.
   */
  void updateWeights() const;

  /**
   * @brief Bind layers together.
   *
   */
  void bindLayers();

  /**
   * @brief Initialize layers
   *
   */
  void initializeLayers();

  /**
   * @brief Initialize weights.
   *
   */
  void initializeWeights() const;

  /**
   * @brief Set the Activation Function on a layer
   *
   * @param layer
   * @param activation_function
   * @param activation_alpha
   */
  void SetActivationFunction(Layer *layer,
                             EActivationFunction activation_function,
                             float activation_alpha) const;
};
} // namespace smlp