#include "NetworkImportExportJSON.h"

using namespace smlp;

Network *NetworkImportExportJSON::importModel(const AppParameters &app_params) {
  using json = nlohmann::json;
  const auto &logger = SimpleLogger::getInstance();
  const auto &lang = SimpleLang::getInstance();

  if (app_params.network_to_import.empty()) {
    throw ImportExportException(lang.get(Error::MissingImportFile));
  }

  std::string path_in_ext = app_params.network_to_import;
  if (std::filesystem::path p(path_in_ext); p.parent_path().empty()) {
    path_in_ext = "./" + path_in_ext;
  }

  std::ifstream file(path_in_ext);
  json json_model;

  if (!file.is_open()) {
    throw ImportExportException(lang.get(Error::FailedToOpenFile) + ": " +
                                path_in_ext);
  }

  if (!json::accept(file)) {
    file.close();
    throw ImportExportException(lang.get(Error::JsonParsingError) + ": " +
                                path_in_ext);
  }
  file.seekg(0, std::ifstream::beg);

  try {
    json_model = json::parse(file);

    if (std::string jversion = json_model["version"];
        jversion != app_params.version) {
      logger.warn(
          lang.get(Error::DifferentModelVersion,
                   {{"vuser", jversion}, {"vcurrent", app_params.version}}));
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
        throw ImportExportException(lang.get(Error::LayerTypeNotRecognized));
      }

      // Add neurons without their weights
      std::ranges::for_each(json_layer["neurons"], [&layer](const auto &) {
        layer->neurons.emplace_back();
      });

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
        throw ImportExportException(lang.get(Error::LayerTypeNotRecognized));
      }

      // Add the layer to the network.
      model->layers.push_back(layer);
    }

    if (model->layers.front()->layerType != LayerType::InputLayer) {
      throw ImportExportException(lang.get(Error::LayerTypeInvalidInput));
    }

    if (model->layers.back()->layerType != LayerType::OutputLayer) {
      throw ImportExportException(lang.get(Error::LayerTypeInvalidOutput));
    }

    model->bindLayers();

    file.close();

    return model;

  } catch (const nlohmann::json::parse_error &e) {
    file.close();
    throw ImportExportException(lang.get(Error::JsonParsingError) + ": " +
                                std::string(e.what()));
  }
}

void NetworkImportExportJSON::exportModel(
    const Network *network, const AppParameters &app_params) const {
  using json = nlohmann::json;
  json json_network;
  json_network["version"] = app_params.version;

  // Serialize the layers to JSON.
  for (auto layer : network->layers) {
    json json_layer = {{"type", layer->layerTypeStr()},
                       {"neurons", json::array()}};

    std::ranges::for_each(layer->neurons, [&json_layer](const auto &) {
      json_layer["neurons"].emplace_back();
    });

    json_network["layers"].push_back(json_layer);
  }

  // Serialize the parameters to JSON.
  json_network["parameters"]["input_size"] = json(network->params.input_size);
  json_network["parameters"]["hidden_size"] = json(network->params.hidden_size);
  json_network["parameters"]["output_size"] = json(network->params.output_size);
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