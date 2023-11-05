/**
 * @file ImpotExportJson.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Import/Export using JSON format
 * @see https://github.com/nlohmann/json
 * @date 2023-10-29
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "../../json/include/json.hpp"
#include "Common.h"
#include "Layer.h"
#include "Network.h"
#include "OutputLayer.h"
#include "SimpleLogger.h"
#include "exception/ImportExportJSONException.h"
#include <filesystem>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

/**
 * @brief ImportExportJSON class to export and import network models using the
 * JSON format.
 *
 */
class ImportExportJSON {
public:
  void setLogger(const SimpleLogger &logger) { logger_ = logger; }

  /**
   * @brief Parse a json file into a network model.
   *
   * @param app_params the application parameters, including the file path/
   * @return Network*
   */
  Network *importModel(const AppParameters &app_params) {
    if (app_params.network_to_import.empty()) {
      throw ImportExportJSONException("Missing file to import.");
    }

    std::string path_in_ext = app_params.network_to_import;
    if (std::filesystem::path p(path_in_ext); p.parent_path().empty()) {
      path_in_ext = "./" + path_in_ext;
    }

    std::ifstream file(path_in_ext);
    json json_model;

    if (!file.is_open()) {
      throw ImportExportJSONException("Failed to open file for reading.");
    }

    if (!json::accept(file)) {
      file.close();
      throw ImportExportJSONException("JSON parsing error: invalid JSON file.");
    }
    file.seekg(0, std::ifstream::beg);

    try {
      json_model = json::parse(file);

      if (std::string jversion = json_model["version"];
          jversion != app_params.version) {
        logger_.warn("Your model version (", jversion,
                     ") is not the same as current version (",
                     app_params.version, ")");
      }

      // Create a new Network object and deserialize the JSON data into it.
      auto model = new Network();
      model->params.input_size = json_model["parameters"]["input_size"];
      model->params.hidden_size = json_model["parameters"]["hidden_size"];
      model->params.output_size = json_model["parameters"]["output_size"];
      model->params.hiddens_count = json_model["parameters"]["hiddens_count"];
      model->params.learning_rate = json_model["parameters"]["learning_rate"];
      model->params.hidden_activation_alpha =
          json_model["parameters"]["hidden_activation_alpha"];
      model->params.output_activation_alpha =
          json_model["parameters"]["output_activation_alpha"];
      model->params.hidden_activation_function =
          json_model["parameters"]["hidden_activation_function"];
      model->params.output_activation_function =
          json_model["parameters"]["output_activation_function"];

      for (auto json_layer : json_model["layers"]) {
        // Get the type of the layer.
        std::string layer_type_str = json_layer["type"];
        LayerType layer_type = layer_map.at(layer_type_str);

        // Create a new layer object of the appropriate type.
        Layer *layer = nullptr;
        switch (layer_type) {
        case LayerType::InputLayer:
          layer = new InputLayer();
          break;
        case LayerType::HiddenLayer:
          layer = new HiddenLayer();
          break;
        case LayerType::OutputLayer:
          layer = new OutputLayer();
          break;
        default:
          throw ImportExportJSONException("LayerType not recognized.");
        }

        // Add neurons and their weights
        for (auto json_neuron : json_layer["neurons"]) {
          Neuron n;
          json_neuron["weights"].get_to(n.weights);
          layer->neurons.push_back(n);
        }

        // Set activation functions
        switch (layer->layerType) {
        case LayerType::InputLayer: // no activation function here
          break;
        case LayerType::HiddenLayer:
          model->SetActivationFunction(layer,
                                       model->params.hidden_activation_function,
                                       model->params.hidden_activation_alpha);
          break;
        case LayerType::OutputLayer:
          model->SetActivationFunction(layer,
                                       model->params.output_activation_function,
                                       model->params.output_activation_alpha);
          break;
        default:
          throw ImportExportJSONException("LayerType not recognized.");
        }

        // Add the layer to the network.
        model->layers.push_back(layer);
      }

      if (model->layers.front()->layerType != LayerType::InputLayer) {
        throw ImportExportJSONException(
            "Invalid LayerType for first layer: not an InputLayer");
      }

      if (model->layers.back()->layerType != LayerType::OutputLayer) {
        throw ImportExportJSONException(
            "Invalid LayerType for last layer: not an OutputLayer");
      }

      model->bindLayers();

      file.close();

      return model;

    } catch (const nlohmann::json::parse_error &e) {
      file.close();
      throw ImportExportJSONException("JSON parsing error: " +
                                      std::string(e.what()));
    }
  }

  json createJsonNeuron(const Neuron &neuron, LayerType layerType) const {
    json json_neuron = {{"weights", json::array()}};

    if (layerType != LayerType::InputLayer) {
      for (auto weight : neuron.weights) {
        json_neuron["weights"].push_back(weight);
      }
    }

    return json_neuron;
  }

  /**
   * @brief export a Network model into a json file.
   *
   * @param network the network to export.
   * @param app_params the application parameters.
   */
  void exportModel(const Network *network,
                   const AppParameters &app_params) const {
    json json_network;
    json_network["version"] = app_params.version;

    // Serialize the layers to JSON.
    for (auto layer : network->layers) {
      json json_layer = {{"type", layer->layerTypeStr()},
                         {"neurons", json::array()}};

      for (const auto &neuron : layer->neurons) {
        json_layer["neurons"].push_back(
            createJsonNeuron(neuron, layer->layerType));
      }

      json_network["layers"].push_back(json_layer);
    }

    // Serialize the parameters to JSON.
    json_network["parameters"]["input_size"] = json(network->params.input_size);
    json_network["parameters"]["hidden_size"] =
        json(network->params.hidden_size);
    json_network["parameters"]["output_size"] =
        json(network->params.output_size);
    json_network["parameters"]["hiddens_count"] =
        json(network->params.hiddens_count);
    json_network["parameters"]["learning_rate"] =
        json(network->params.learning_rate);
    json_network["parameters"]["hidden_activation_alpha"] =
        json(network->params.hidden_activation_alpha);
    json_network["parameters"]["output_activation_alpha"] =
        json(network->params.output_activation_alpha);
    json_network["parameters"]["hidden_activation_function"] =
        json(network->params.hidden_activation_function);
    json_network["parameters"]["output_activation_function"] =
        json(network->params.output_activation_function);

    // Write the JSON object to the file.
    // The 4 argument specifies the indentation level of the resulting string.
    std::ofstream file(app_params.network_to_export);
    file << json_network.dump(2);
    file.close();
  }

private:
  [[no_unique_address]] SimpleLogger logger_;
};