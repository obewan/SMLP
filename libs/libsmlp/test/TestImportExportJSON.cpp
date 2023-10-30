#include "ImportExportJSON.h"
#include "Layer.h"
#include "doctest.h"
#include <cstddef>
#include <filesystem>

TEST_CASE("Testing the ImportExportJSON class") {
  std::string test_file = "../../../../libs/libsmlp/test/data/test_file.csv";

  Parameters params{.data_file = test_file, .input_size = 20, .output_size = 1};
  std::string modelJsonFile =
      "./testModel.json"; // beware path "./" is important or else parsing
                          // error.

  SUBCASE("Test exportModel function") {
    if (std::filesystem::exists(modelJsonFile)) {
      std::filesystem::remove(modelJsonFile);
    }
    CHECK(std::filesystem::exists(modelJsonFile) == false);

    ImportExportJSON importExportJSON;
    auto network = new Network(params);
    CHECK_NOTHROW(importExportJSON.exportModel(modelJsonFile, network));

    CHECK(std::filesystem::exists(modelJsonFile) == true);
  }

  SUBCASE("Test importModel function") {
    CHECK(std::filesystem::exists(modelJsonFile) == true);

    ImportExportJSON importExportJSON;
    Network *network = nullptr;
    CHECK_NOTHROW(network = importExportJSON.importModel(modelJsonFile));
    CHECK(network != nullptr);
    CHECK(network->layers.front()->layerType == LayerType::InputLayer);
    CHECK(network->layers.front()->neurons.size() == params.input_size);
    CHECK(network->layers.back()->layerType == LayerType::OutputLayer);
    CHECK(network->layers.back()->neurons.size() == params.output_size);
  }
}