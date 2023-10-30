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
#include "exception/ImportExportJSONException.h"
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
  /**
   * @brief Parse a json file into a network model.
   *
   * @param path_in the file path, including its path (relative or absolute),
   * not just a filename or else it will failed to parse.
   * @return Network*
   */
  Network *importModel(const std::string &path_in,
                       const AppParameters &app_params) {
    std::string path_in_ext = path_in;

    // A workaround for parsing error in case of missing
    // at least a relative path
    if (path_in.find('/') == std::string::npos &&
        path_in.find('\\') == std::string::npos) {
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
        std::cerr << "[WARN] your model version (" << jversion
                  << ") is not the same as current version ("
                  << app_params.version << ")" << std::endl;
      }

      // Create a new Network object and deserialize the JSON data into it.
      auto model = new Network();
      model->params.input_size = json_model["parameters"]["input_size"];
      model->params.hidden_size = json_model["parameters"]["hidden_size"];
      model->params.output_size = json_model["parameters"]["output_size"];
      model->params.hiddens_count = json_model["parameters"]["hiddens_count"];
      model->params.num_epochs = json_model["parameters"]["num_epochs"];
      model->params.output_index_to_monitor =
          json_model["parameters"]["output_index_to_monitor"];
      model->params.training_ratio = json_model["parameters"]["training_ratio"];
      model->params.learning_rate = json_model["parameters"]["learning_rate"];
      model->params.output_at_end = json_model["parameters"]["output_at_end"];
      model->params.verbose = json_model["parameters"]["verbose"];
      model->params.mode = json_model["parameters"]["mode"];

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

      file.close();

      return model;

    } catch (const nlohmann::json::parse_error &e) {
      file.close();
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
  void exportModel(const std::string &path_out, const Network *network,
                   const AppParameters &app_params) const {
    json json_network;
    json_network["version"] = app_params.version;

    // Serialize the layers to JSON.
    for (auto layer : network->layers) {
      json json_layer = {{"type", layer->layerTypeStr()},
                         {"neurons", json::array()}};

      for (const auto &neuron : layer->neurons) {
        json json_neuron = {{"weights", json::array()}};

        for (auto weight : neuron.weights) {
          json_neuron["weights"].push_back(weight);
        }

        json_layer["neurons"].push_back(json_neuron);
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
    json_network["parameters"]["num_epochs"] = json(network->params.num_epochs);
    json_network["parameters"]["output_index_to_monitor"] =
        json(network->params.output_index_to_monitor);
    json_network["parameters"]["training_ratio"] =
        json(network->params.training_ratio);
    json_network["parameters"]["learning_rate"] =
        json(network->params.learning_rate);
    json_network["parameters"]["output_at_end"] =
        json(network->params.output_at_end);
    json_network["parameters"]["verbose"] = json(network->params.verbose);
    json_network["parameters"]["mode"] = json(network->params.mode);

    // Write the JSON object to the file.
    // The 4 argument specifies the indentation level of the resulting string.
    std::ofstream file(path_out);
    file << json_network.dump(4);
    file.close();
  }
};