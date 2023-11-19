#include "Common.h"
#include "Layer.h"
#include "NetworkImportExportJSON.h"
#include "doctest.h"
#include "exception/ImportExportException.h"
#include <cstddef>
#include <filesystem>

TEST_CASE("Testing the ImportExportJSON class") {
  std::string test_file = "data/test_file.csv";
  const float eps = 1e-6f; // epsilon for float testing

  NetworkParameters params{.input_size = 20,
                           .hidden_size = 12,
                           .output_size = 1,
                           .hiddens_count = 1};
  AppParameters app_params{.version = "1.0.0", .data_file = test_file};
  std::string modelJsonFile = "testModel.json";

  SUBCASE("Test exceptions") {
    NetworkImportExportJSON importExportJSON;
    CHECK_THROWS_AS(
        importExportJSON.importModel({.network_to_import = "wrongfile"}),
        ImportExportException);
  }

  SUBCASE("Test exportModel function") {
    if (std::filesystem::exists(modelJsonFile)) {
      std::filesystem::remove(modelJsonFile);
    }
    CHECK(std::filesystem::exists(modelJsonFile) == false);
    app_params.network_to_export = modelJsonFile;

    NetworkImportExportJSON importExportJSON;
    auto network = new Network(params);
    CHECK_NOTHROW(importExportJSON.exportModel(network, app_params));

    CHECK(std::filesystem::exists(modelJsonFile) == true);
  }

  SUBCASE("Test importModel function") {
    CHECK(std::filesystem::exists(modelJsonFile) == true);

    NetworkImportExportJSON importExportJSON;
    app_params.network_to_import = modelJsonFile;
    Network *network = nullptr;
    CHECK_NOTHROW(network = importExportJSON.importModel(app_params));
    CHECK(network != nullptr);
    CHECK(network->layers.front()->layerType == LayerType::InputLayer);
    CHECK(network->layers.front()->neurons.size() == params.input_size);
    CHECK(network->layers.back()->layerType == LayerType::OutputLayer);
    CHECK(network->layers.back()->neurons.size() == params.output_size);
    CHECK(network->params.input_size == params.input_size);
    CHECK(network->params.hidden_size == params.hidden_size);
    CHECK(network->params.output_size == params.output_size);
    CHECK(network->params.hiddens_count == params.hiddens_count);
    CHECK(network->params.learning_rate ==
          doctest::Approx(params.learning_rate).epsilon(eps));
  }
}