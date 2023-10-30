#include "Common.h"
#include "ImportExportJSON.h"
#include "Layer.h"
#include "doctest.h"
#include <cstddef>
#include <filesystem>

TEST_CASE("Testing the ImportExportJSON class") {
  std::string test_file = "../../../../libs/libsmlp/test/data/test_file.csv";

  NetworkParameters params{
      .data_file = test_file,
      .input_size = 20,
      .hidden_size = 12,
      .output_size = 1,
      .hiddens_count = 1,
      .num_epochs = 2,
      .output_at_end = true,
      .mode = Mode::TrainTestMonitored,
  };
  AppParameters app_params{.version = "1.0.0"};
  std::string modelJsonFile = "testModel.json";

  SUBCASE("Test exportModel function") {
    if (std::filesystem::exists(modelJsonFile)) {
      std::filesystem::remove(modelJsonFile);
    }
    CHECK(std::filesystem::exists(modelJsonFile) == false);

    ImportExportJSON importExportJSON;
    auto network = new Network(params);
    CHECK_NOTHROW(
        importExportJSON.exportModel(modelJsonFile, network, app_params));

    CHECK(std::filesystem::exists(modelJsonFile) == true);
  }

  SUBCASE("Test importModel function") {
    CHECK(std::filesystem::exists(modelJsonFile) == true);

    ImportExportJSON importExportJSON;
    Network *network = nullptr;
    CHECK_NOTHROW(network =
                      importExportJSON.importModel(modelJsonFile, app_params));
    CHECK(network != nullptr);
    CHECK(network->layers.front()->layerType == LayerType::InputLayer);
    CHECK(network->layers.front()->neurons.size() == params.input_size);
    CHECK(network->layers.back()->layerType == LayerType::OutputLayer);
    CHECK(network->layers.back()->neurons.size() == params.output_size);
    CHECK(network->params.data_file == params.data_file);
    CHECK(network->params.input_size == params.input_size);
    CHECK(network->params.hidden_size == params.hidden_size);
    CHECK(network->params.output_size == params.output_size);
    CHECK(network->params.hiddens_count == params.hiddens_count);
    CHECK(network->params.num_epochs == params.num_epochs);
    CHECK(network->params.output_index_to_monitor ==
          params.output_index_to_monitor);
    CHECK(abs(network->params.training_ratio - params.training_ratio) < 1e-6f);
    CHECK(abs(network->params.learning_rate - params.learning_rate) < 1e-6f);
    CHECK(network->params.output_at_end == params.output_at_end);
    CHECK(network->params.verbose == params.verbose);
    CHECK(network->params.mode == params.mode);
  }
}