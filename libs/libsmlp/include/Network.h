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
#include "HiddenLayer.h"
#include "InputLayer.h"
#include "OutputLayer.h"

/**
 The methods for getting and setting weights and biases are also necessary
 for implementing backpropagation during training.
 The addition of InitializeWeights() will also be useful
 for initializing the weights in the network before training.

 The Forward() method takes a std::vector<float> input and returns a float*
output. It's important to make sure that the memory for the output is allocated
 before passing it to the ComputeOutput() method of the output layer.

The Backward() and ClearGradient() methods will be used during backpropagation
to compute the gradients of the weights and biases in the network.

Finally, the Predict() method takes a std::vector<float> input and returns a
std::vector<float> output. This method will be useful for using the trained
network to make predictions on new input data.
*/

class Network {
public:
  InputLayer inputLayer;
  OutputLayer outputLayer;
  std::vector<HiddenLayer> hiddenLayers;
  float learningRate;

  Network(size_t inputSize, size_t hiddenLayerSize, size_t outputLayerSize,
          size_t totalHiddenLayers) {
    // setting layers
    inputLayer.neurons.resize(inputSize);
    hiddenLayers.resize(totalHiddenLayers);
    for (auto &hl : hiddenLayers) {
      hl.neurons.resize(hiddenLayerSize);
    }
    outputLayer.neurons.resize(outputLayerSize);

    // setting neurons weights with previous layer size
    if (hiddenLayerSize > 0) {
      for (size_t i = 0; i < totalHiddenLayers; i++) {
        for (auto &n : hiddenLayers.at(i).neurons) {
          n.initWeights(i == 0 ? inputSize : hiddenLayerSize);
        }
      }
      for (auto &n : outputLayer.neurons) {
        n.initWeights(hiddenLayerSize);
      }
    } else {
      for (auto &n : outputLayer.neurons) {
        n.initWeights(inputSize);
      }
    }
  }

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

  void updateWeights() {
    // Implement update weights for network
    if (hiddenLayers.empty()) {
      outputLayer.updateWeights(inputLayer, learningRate);
    } else {
      hiddenLayers.front().updateWeights(inputLayer, learningRate);
      if (hiddenLayers.size() > 1) {
        for (size_t i = 1; i < hiddenLayers.size(); i++)
          hiddenLayers.at(i).updateWeights(hiddenLayers.at(i - 1),
                                           learningRate);
      }
      outputLayer.updateWeights(hiddenLayers.back(), learningRate);
    }
  }
};
