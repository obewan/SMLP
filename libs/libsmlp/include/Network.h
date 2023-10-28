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
#include "Common.h"
#include "HiddenLayer.h"
#include "InputLayer.h"
#include "OutputLayer.h"

/**
 * @brief The Network class represents a neural network model. It contains an
 * input layer, an output layer, and a vector of hidden layers. It also has a
 * learning rate and methods for forward propagation, backward propagation, and
 * updating weights.
 */
class Network {
public:
  // The input layer of the network
  InputLayer inputLayer;

  // The output layer of the network
  OutputLayer outputLayer;

  // The hidden layers of the network
  std::vector<HiddenLayer> hiddenLayers;

  // Network parameters
  Parameters params;

  /**
   * @brief Constructor that takes a Parameters object as an argument. It
   * initializes the layers and neurons in the network.
   *
   * @param params The parameters for initializing the network.
   */
  explicit Network(const Parameters &params) : params(params) {
    // setting layers
    inputLayer.neurons.resize(params.input_size);
    hiddenLayers.resize(params.hiddens_count);
    for (auto &hl : hiddenLayers) {
      hl.neurons.resize(params.hidden_size);
    }
    outputLayer.neurons.resize(params.output_size);

    // setting neurons weights with previous layer size
    if (params.hidden_size > 0) {
      for (size_t i = 0; i < params.hiddens_count; i++) {
        for (auto &n : hiddenLayers.at(i).neurons) {
          n.initWeights(i == 0 ? params.input_size : params.hidden_size);
        }
      }
      for (auto &n : outputLayer.neurons) {
        n.initWeights(params.hidden_size);
      }
    } else {
      for (auto &n : outputLayer.neurons) {
        n.initWeights(params.input_size);
      }
    }
  }

  /**
   * @brief Performs forward propagation on the network using the given input
   * values.
   *
   * @param inputValues The input values for forward propagation.
   * @return A vector of output values from the output layer after forward
   * propagation.
   */
  std::vector<float> forwardPropagation(const std::vector<float> &inputValues) {
    inputLayer.setInputValues(inputValues);
    // Implement forward propagation for network
    if (hiddenLayers.empty()) {
      outputLayer.forwardPropagation(inputLayer);
    } else {
      hiddenLayers.front().forwardPropagation(inputLayer);
      if (hiddenLayers.size() > 1) {
        for (size_t i = 1; i < hiddenLayers.size(); i++)
          hiddenLayers.at(i).forwardPropagation(hiddenLayers.at(i - 1));
      }
      outputLayer.forwardPropagation(hiddenLayers.back());
    }
    return outputLayer.getOutputValues();
  }

  /**
   * @brief Performs backward propagation on the network using the given
   * expected values.
   *
   * @param expectedValues The expected values for backward propagation.
   */
  void backwardPropagation(const std::vector<float> &expectedValues) {
    outputLayer.computeErrors(expectedValues);
    // Implement backward propagation for network
    if (!hiddenLayers.empty()) {
      for (int i = (int)hiddenLayers.size() - 1; i >= 0; --i) {
        if (i == (int)hiddenLayers.size() - 1) {
          hiddenLayers[i].backwardPropagation(outputLayer);
        } else {
          hiddenLayers[i].backwardPropagation(hiddenLayers[i + 1]);
        }
      }
    }
  }

  /**
   * @brief Updates the weights of the neurons in the network using the learning
   * rate.
   */
  void updateWeights() {
    // Implement update weights for network
    if (hiddenLayers.empty()) {
      outputLayer.updateWeights(inputLayer, params.learning_rate);
    } else {
      hiddenLayers.front().updateWeights(inputLayer, params.learning_rate);
      if (hiddenLayers.size() > 1) {
        for (size_t i = 1; i < hiddenLayers.size(); i++)
          hiddenLayers.at(i).updateWeights(hiddenLayers.at(i - 1),
                                           params.learning_rate);
      }
      outputLayer.updateWeights(hiddenLayers.back(), params.learning_rate);
    }
  }
};
