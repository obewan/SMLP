#include "CommonModes.h"
#include "Manager.h"
#include "NetworkImportExportFacade.h"
#include "Predict.h"
#include "PredictFile.h"
#include "doctest.h"
#include "exception/PredictException.h"
#include <cstddef>
#include <filesystem>
#include <memory>

using namespace smlp;

TEST_CASE("Testing the Predict class") {
  SUBCASE("Test constructor") { CHECK_NOTHROW(PredictFile()); }

  SUBCASE("Test predict") {
    std::string modelJsonFile = "testModel.json";
    std::string modelCsvFile = "testModel.csv";
    std::string test_file = "../data/test_file.csv";
    Manager::getInstance().app_params = {
        .version = "1.0.0",
        .network_to_import = modelJsonFile,
        .data_file = test_file,
        .output_at_end = false,
    };
    NetworkImportExportFacade importExport;
    CHECK(std::filesystem::exists(modelJsonFile) == true);
    CHECK(std::filesystem::exists(modelCsvFile) == true);

    const auto &app_params = Manager::getInstance().app_params;
    CHECK_NOTHROW({
      Manager::getInstance().network = importExport.importModel(app_params);
      PredictFile predict;
      predict.predictData();
    });

    CHECK_THROWS_WITH_AS(
        {
          auto invalidEnum = static_cast<EPredictMode>(900);
          Manager::getInstance().app_params.predict_mode = invalidEnum;
          PredictFile predict;
          predict.predictData();
        },
        SimpleLang::Error(Error::UnimplementedPredictMode).c_str(),
        PredictException);

    // Reset
    Manager::getInstance().app_params.predict_mode = EPredictMode::CSV;
  }
}