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

  /**
   * @brief Constructor that takes a Parameters object as an argument. It
   * initializes the layers and neurons in the network.
   *
   * @param params The parameters for initializing the network.
   */
  explicit Network(const NetworkParameters &params) : params(params) {
    initializeLayers();
  }

  std::vector<Layer *> layers;
  NetworkParameters params;

  /**
   * @brief Performs forward propagation on the network using the given input
   * values.
   *
   * @param inputValues The input values for forward propagation.
   * @return A vector of output values from the output layer after forward
   * propagation.
   */
  std::vector<float> forwardPropagation(const std::vector<float> &inputValues) {
    ((InputLayer *)layers.front())->setInputValues(inputValues);
    for (auto &layer : layers) {
      layer->forwardPropagation();
    }
    return ((OutputLayer *)layers.back())->getOutputValues();
  }

  /**
   * @brief Performs backward propagation on the network using the given
   * expected values.
   *
   * @param expectedValues The expected values for backward propagation.
   */
  void backwardPropagation(const std::vector<float> &expectedValues) {
    ((OutputLayer *)layers.back())->computeErrors(expectedValues);
    for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
      (*it)->backwardPropagation();
    }
  }

  /**
   * @brief Updates the weights of the neurons in the network using the learning
   * rate.
   */
  void updateWeights() const {
    for (auto &layer : layers) {
      layer->updateWeights(params.learning_rate);
    }
  }

  void bindLayers() {
    for (size_t i = 0; i < layers.size(); ++i) {
      if (i > 0) {
        layers.at(i)->previousLayer = layers.at(i - 1);
      }
      if (i < layers.size() - 1) {
        layers.at(i)->nextLayer = layers.at(i + 1);
      }
    }
  }

  void initializeWeights() const {
    for (auto layer : layers) {
      if (layer->previousLayer != nullptr) {
        for (auto &n : layer->neurons) {
          n.initWeights(layer->previousLayer->neurons.size());
        }
      }
    }
  }

  void initializeLayers() {
    auto inputLayer = new InputLayer();
    inputLayer->neurons.resize(params.input_size);
    layers.push_back(inputLayer);

    for (size_t i = 0; i < params.hiddens_count; ++i) {
      auto hiddenLayer = new HiddenLayer();
      hiddenLayer->neurons.resize(params.hidden_size);
      SetActivationFunction(hiddenLayer, params.hidden_activation_function,
                            params.hidden_activation_alpha);
      layers.push_back(hiddenLayer);
    }

    auto outputLayer = new OutputLayer();
    outputLayer->neurons.resize(params.output_size);
    SetActivationFunction(outputLayer, params.output_activation_function,
                          params.output_activation_alpha);
    layers.push_back(outputLayer);

    bindLayers();
    initializeWeights();
  }

  void SetActivationFunction(Layer *layer,
                             EActivationFunction activation_function,
                             float activation_alpha) const {
    switch (activation_function) {
    case EActivationFunction::ELU:
      layer->setActivationFunction(
          [activation_alpha](auto x) { return elu(x, activation_alpha); },
          [activation_alpha](auto x) {
            return eluDerivative(x, activation_alpha);
          });
      break;
    case EActivationFunction::LReLU:
      layer->setActivationFunction(leakyRelu, leakyReluDerivative);
      break;
    case EActivationFunction::PReLU:
      layer->setActivationFunction(
          [activation_alpha](auto x) {
            return parametricRelu(x, activation_alpha);
          },
          [activation_alpha](auto x) {
            return parametricReluDerivative(x, activation_alpha);
          });
      break;
    case EActivationFunction::ReLU:
      layer->setActivationFunction(relu, reluDerivative);
      break;
    case EActivationFunction::Sigmoid:
      layer->setActivationFunction(sigmoid, sigmoidDerivative);
      break;
    case EActivationFunction::Tanh:
      layer->setActivationFunction(tanhFunc, tanhDerivative);
      break;
    default:
      throw NetworkException(
          SimpleLang::Error(Error::UnimplementedActivationFunction));
    }
  }
};
