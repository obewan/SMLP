#include "Network.h"
#include "Manager.h"

using namespace smlp;

std::vector<float>
Network::forwardPropagation(const std::vector<float> &inputValues) {
  ((InputLayer *)layers.front())->setInputValues(inputValues);
  for (auto &layer : layers) {
    layer->forwardPropagation();
  }
  return ((OutputLayer *)layers.back())->getOutputValues();
}

void Network::backwardPropagation(const std::vector<float> &expectedValues) {
  ((OutputLayer *)layers.back())->computeErrors(expectedValues);
  for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
    (*it)->backwardPropagation();
  }
}

void Network::updateWeights() const {
  const auto &params = Manager::getInstance().network_params;
  for (auto &layer : layers) {
    layer->updateWeights(params.learning_rate);
  }
}

void Network::bindLayers() {
  for (size_t i = 0; i < layers.size(); ++i) {
    if (i > 0) {
      layers.at(i)->previousLayer = layers.at(i - 1);
    }
    if (i < layers.size() - 1) {
      layers.at(i)->nextLayer = layers.at(i + 1);
    }
  }
}

void Network::initializeLayers() {
  const auto &params = Manager::getInstance().network_params;
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

void Network::initializeWeights() const {
  for (auto layer : layers) {
    if (layer->previousLayer != nullptr) {
      for (auto &n : layer->neurons) {
        n.initWeights(layer->previousLayer->neurons.size());
      }
    }
  }
}

void Network::SetActivationFunction(Layer *layer,
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