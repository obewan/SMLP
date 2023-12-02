#include "NetworkImportExportJSON.h"
#include "Predict.h"
#include "doctest.h"
#include "exception/PredictException.h"
#include <cstddef>
#include <filesystem>
#include <memory>

TEST_CASE("Testing the Predict class") {
  SUBCASE("Test constructor") { CHECK_NOTHROW(Predict(nullptr, {})); }

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

    std::shared_ptr<Network> network = nullptr;
    CHECK_NOTHROW({
      network =
          std::shared_ptr<Network>(importExportJSON.importModel(app_params));
      Predict predict(network, app_params);
      predict.predict();
    });

    CHECK_THROWS_WITH_AS(
        {
          auto invalidEnum = static_cast<EPredictiveMode>(900);
          app_params.predictive_mode = invalidEnum;
          network = std::shared_ptr<Network>(
              importExportJSON.importModel(app_params));
          Predict predict(network, app_params);
          predict.predict();
        },
        SimpleLang::Error(Error::UnimplementedPredictiveMode).c_str(),
        PredictException);
  }
}