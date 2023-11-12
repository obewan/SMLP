#include "NetworkImportExportJSON.h"
#include "Predict.h"
#include "doctest.h"
#include <filesystem>

TEST_CASE("Testing the Predict class") {
  SUBCASE("Test constructor") {
    CHECK_NOTHROW(Predict(nullptr, {}, SimpleLogger()));
  }

  SUBCASE("Test predict") {
    std::string modelJsonFile = "testModel.json";
    std::string test_file = "../data/test_file.csv";
    AppParameters app_params{
        .version = "1.0.0",
        .network_to_import = modelJsonFile,
        .data_file = test_file,
        .output_at_end = false,
    };
    NetworkImportExportJSON importExportJSON;
    CHECK(std::filesystem::exists(modelJsonFile) == true);

    Network *network = nullptr;
    CHECK_NOTHROW(network = importExportJSON.importModel(app_params));
    Predict predict(network, app_params, SimpleLogger());
    CHECK_NOTHROW(predict.predict());
  }
}