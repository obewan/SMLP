#include "Common.h"
#include "Layer.h"
#include "NetworkImportExport.h"
#include "doctest.h"
#include "exception/ImportExportException.h"
#include <cstddef>
#include <filesystem>

using namespace smlp;

TEST_CASE("Testing the ImportExport") {
  std::string test_file = "data/test_file.csv";
  const float eps = 1e-6f; // epsilon for float testing

  NetworkParameters params{.input_size = 20,
                           .hidden_size = 12,
                           .output_size = 1,
                           .hiddens_count = 1};
  AppParameters app_params{.version = "1.0.0", .data_file = test_file};
  std::string modelJsonFile = "testModel.json";
  std::string modelCsvFile = "testModel.csv";

  SUBCASE("Test exceptions") {
    NetworkImportExport importExport;
    CHECK_THROWS_AS(
        importExport.importModel({.network_to_import = "wrongfile"}),
        ImportExportException);
  }

  SUBCASE("Test exportModel function") {
    if (std::filesystem::exists(modelJsonFile)) {
      std::filesystem::remove(modelJsonFile);
    }
    if (std::filesystem::exists(modelCsvFile)) {
      std::filesystem::remove(modelCsvFile);
    }
    CHECK(std::filesystem::exists(modelJsonFile) == false);
    CHECK(std::filesystem::exists(modelCsvFile) == false);
    app_params.network_to_export = modelJsonFile;

    NetworkImportExport importExport;
    auto network = new Network(params);
    CHECK_NOTHROW(importExport.exportModel(network, app_params));

    CHECK(std::filesystem::exists(modelJsonFile) == true);
    CHECK(std::filesystem::exists(modelCsvFile) == true);
  }

  SUBCASE("Test importModel function") {
    CHECK(std::filesystem::exists(modelJsonFile) == true);
    CHECK(std::filesystem::exists(modelCsvFile) == true);

    NetworkImportExport importExport;
    app_params.network_to_import = modelJsonFile;
    Network *network = nullptr;
    CHECK_NOTHROW(network = importExport.importModel(app_params));
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