/**
 * @file ImpotExportJson.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Import/Export using JSON format
 * @date 2023-10-29
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "../../json/include/json.hpp"
#include "Layer.h"
#include "Network.h"
#include "OutputLayer.h"
#include "exception/ImportExportJSONException.h"
#include <fstream>

using json = nlohmann::json;

/**
 * @brief ImportExportJSON class to export and import network models using the
 * JSON format.
 *
 */
class ImportExportJSON {
public:
  std::string version = "1.0.0";

  /**
   * @brief Parse a json file into a network model.
   *
   * @param path_in the file path, including its path (relative or absolute),
   * not just a filename or else it will failed to parse.
   * @return Network*
   */
  Network *importModel(const std::string &path_in) {
    std::ifstream file(path_in);
    nlohmann::json json_model;

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

      // Create a new Network object and deserialize the JSON data into it.
      auto model = new Network();
      model->params.learning_rate = json_model["params"]["learning_rate"];

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

        // Deserialize the JSON data for the layer into it.
        for (auto json_neuron : json_layer["neurons"]) {
          Neuron n;
          json_neuron["weights"].get_to(n.weights);
          layer->neurons.push_back(n);
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

      return model;

    } catch (const nlohmann::json::parse_error &e) {
      throw ImportExportJSONException("JSON parsing error: " +
                                      std::string(e.what()));
    }
  }

  /**
   * @brief export a Network model into a json file.
   *
   * @param path_out path of the file to create, including its filename.
   * @param network the network to export.
   */
  void exportModel(const std::string &path_out, const Network *network) {
    json json_network;
    json_network["version"] = version;

    // Serialize the layers to JSON.
    for (auto layer : network->layers) {
      nlohmann::json json_layer = {{"type", layer->layerTypeStr()},
                                   {"neurons", nlohmann::json::array()}};

      for (const auto &neuron : layer->neurons) {
        nlohmann::json json_neuron = {{"weights", nlohmann::json::array()}};

        for (auto weight : neuron.weights) {
          json_neuron["weights"].push_back(weight);
        }

        json_layer["neurons"].push_back(json_neuron);
      }

      json_network["layers"].push_back(json_layer);
    }

    // Serialize the parameters to JSON.
    json_network["params"] =
        nlohmann::json{{"learning_rate", network->params.learning_rate}};

    // Write the JSON object to the file.
    // The 4 argument specifies the indentation level of the resulting string.
    std::ofstream file(path_out);
    file << json_network.dump(4);
    file.close();
  }
};