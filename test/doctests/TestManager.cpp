#include "CommonErrors.h"
#include "CommonModes.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "Testing.h"
#include "Training.h"
#include "doctest.h"
#include "exception/FileParserException.h"
#include "exception/ManagerException.h"
#include <filesystem>

TEST_CASE("Testing the Manager class") {
  SUBCASE("Test constructor") {
    CHECK_NOTHROW({
      auto &manager = Manager::getInstance();
      auto &logger = manager.logger;
      logger.info("Test ok");
    });
  }

  auto &manager = Manager::getInstance();

  SUBCASE("Test createTraining") {
    manager.app_params.use_socket = true;
    manager.app_params.use_stdin = false;
    CHECK_NOTHROW(manager.createTraining());
    CHECK(manager.getTraining()->trainingType == TrainingType::TrainingSocket);
    manager.resetTraining();

    manager.app_params.use_socket = false;
    manager.app_params.use_stdin = true;
    CHECK_NOTHROW(manager.createTraining());
    CHECK(manager.getTraining()->trainingType == TrainingType::TrainingStdin);
    manager.resetTraining();

    manager.app_params.use_socket = false;
    manager.app_params.use_stdin = false;
    CHECK_NOTHROW(manager.createTraining());
    CHECK(manager.getTraining()->trainingType == TrainingType::TrainingFile);
    manager.resetTraining();
  }

  SUBCASE("Test createTesting") {
    manager.app_params.use_socket = true;
    manager.app_params.use_stdin = false;
    CHECK_NOTHROW(manager.createTesting());
    CHECK(manager.getTesting()->testingType == TestingType::TestingSocket);
    manager.resetTesting();

    manager.app_params.use_socket = false;
    manager.app_params.use_stdin = true;
    CHECK_NOTHROW(manager.createTesting());
    CHECK(manager.getTesting()->testingType == TestingType::TestingStdin);
    manager.resetTesting();

    manager.app_params.use_socket = false;
    manager.app_params.use_stdin = false;
    CHECK_NOTHROW(manager.createTesting());
    CHECK(manager.getTesting()->testingType == TestingType::TestingFile);
    manager.resetTesting();
  }

  SUBCASE("Test file predict") {
    manager.app_params.use_socket = false;
    manager.app_params.use_stdin = false;
    manager.app_params.data_file = "";
    manager.app_params.mode = EMode::Predictive;

    // Test predict file but without a valid file path.
    CHECK_THROWS_WITH_AS(manager.predict(),
                         SimpleLang::Error(Error::FailedToOpenFile).c_str(),
                         FileParserException);

    // Test predict file with a valid file path and network.
    manager.app_params.network_to_import = "testModel.json";
    manager.app_params.data_file = "../data/test_file.csv";
    CHECK(std::filesystem::exists(manager.app_params.network_to_import));
    CHECK(std::filesystem::exists(manager.app_params.data_file));
    CHECK_NOTHROW(manager.importOrBuildNetwork());
    CHECK(manager.network != nullptr);
    CHECK_NOTHROW(manager.predict());
  }

  SUBCASE("Test file training") { CHECK_NOTHROW(manager.train()); }

  SUBCASE("Test file testing") { CHECK_NOTHROW(manager.test()); }

  SUBCASE("Test should export network") {
    manager.app_params.network_to_export = "";
    CHECK(manager.shouldExportNetwork() == false);
    manager.app_params.network_to_export = "fakeFile";
    manager.app_params.mode = EMode::TrainOnly;
    CHECK(manager.shouldExportNetwork() == true);
  }

  SUBCASE("Test procesTCPClient") {
    manager.app_params.use_socket = false;
    CHECK_THROWS_WITH_AS(manager.processTCPClient(""), "TCP socket not set.",
                         ManagerException);
  }
}