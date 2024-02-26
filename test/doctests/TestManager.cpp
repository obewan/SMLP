#include "CommonErrors.h"
#include "CommonModes.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "Testing.h"
#include "Training.h"
#include "doctest.h"
#include "exception/DataParserException.h"
#include "exception/ManagerException.h"
#include <filesystem>

using namespace smlp;

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
    manager.app_params.input = EInput::Socket;
    CHECK_NOTHROW(manager.createTraining());
    CHECK(manager.getTraining()->trainingType == TrainingType::TrainingSocket);
    manager.resetTraining();

    manager.app_params.input = EInput::Stdin;
    CHECK_NOTHROW(manager.createTraining());
    CHECK(manager.getTraining()->trainingType == TrainingType::TrainingStdin);
    manager.resetTraining();

    manager.app_params.input = EInput::File;
    CHECK_NOTHROW(manager.createTraining());
    CHECK(manager.getTraining()->trainingType == TrainingType::TrainingFile);
    manager.resetTraining();
  }

  SUBCASE("Test createTesting") {
    manager.app_params.input = EInput::Socket;
    CHECK_NOTHROW(manager.createTesting());
    CHECK(manager.getTesting()->testingType == TestingType::TestingSocket);
    manager.resetTesting();

    manager.app_params.input = EInput::Stdin;
    CHECK_NOTHROW(manager.createTesting());
    CHECK(manager.getTesting()->testingType == TestingType::TestingStdin);
    manager.resetTesting();

    manager.app_params.input = EInput::File;
    CHECK_NOTHROW(manager.createTesting());
    CHECK(manager.getTesting()->testingType == TestingType::TestingFile);
    manager.resetTesting();
  }

  SUBCASE("Test file predict") {
    manager.app_params.input = EInput::File;
    manager.app_params.data_file = "";
    manager.app_params.mode = EMode::Predict;

    // Test predict file but without a valid file path.
    CHECK_THROWS_WITH_AS(manager.predict(),
                         SimpleLang::Error(Error::FailedToOpenFile).c_str(),
                         DataParserException);

    // Test predict file with a valid file path and network.
    manager.app_params.network_to_import = "../data/testModel.json";
    manager.app_params.data_file = "../data/test_file.csv";
    CHECK(std::filesystem::exists(manager.app_params.network_to_import));
    CHECK(std::filesystem::exists(manager.app_params.data_file));
    CHECK_NOTHROW(manager.importOrBuildNetwork());
    CHECK(manager.network != nullptr);
    CHECK_NOTHROW(manager.predict());
  }

  SUBCASE("Test file training") {
    CHECK_NOTHROW(manager.train());
    CHECK_NOTHROW(manager.trainTestMonitored());
  }

  SUBCASE("Test file testing") { CHECK_NOTHROW(manager.test()); }

  SUBCASE("Test run mode (FILE)") {
    manager.app_params.network_to_import = "../data/testModel.json";
    manager.app_params.data_file = "../data/test_file.csv";
    manager.app_params.verbose = true;
    CHECK(std::filesystem::exists(manager.app_params.network_to_import));
    CHECK(std::filesystem::exists(manager.app_params.data_file));
    CHECK_NOTHROW(manager.importOrBuildNetwork());
    manager.app_params.input = EInput::File;
    manager.app_params.mode = EMode::Predict;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TestOnly;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TrainOnly;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TrainTestMonitored;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TrainThenTest;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.verbose = false;
  }

  SUBCASE("Test run mode (STDIN)") {
    // Save the original buffer
    std::streambuf *cin_buffer = std::cin.rdbuf();
    manager.app_params.network_to_import = "../data/testModel.json";
    CHECK(std::filesystem::exists(manager.app_params.network_to_import));
    CHECK_NOTHROW(manager.importOrBuildNetwork());
    manager.app_params.input = EInput::Stdin;
    manager.app_params.verbose = true;

    std::string inputData =
        "0,0.08,0.43,0.90,0.42,1.00,0.62,0.33,0.38,0.10,0.07,0.00,0.00,0.38,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00\n"
        "1,0.01,0.57,0.90,0.25,1.00,0.00,0.67,0.92,0.09,0.02,0.00, "
        "0.00,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\n";
    std::stringstream inputDataStream(inputData);

    // Redirect std::cin
    std::cin.rdbuf(inputDataStream.rdbuf());

    manager.app_params.mode = EMode::Predict;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TestOnly;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TrainOnly;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TrainTestMonitored;
    CHECK_NOTHROW(manager.runMode());
    manager.app_params.mode = EMode::TrainThenTest;
    CHECK_NOTHROW(manager.runMode());
    // Restore std::cin to normal after the test
    std::cin.rdbuf(cin_buffer);
    manager.app_params.verbose = false;
  }

  SUBCASE("Test export network") {
    manager.app_params.network_to_export = "";
    CHECK(manager.shouldExportNetwork() == false);
    manager.app_params.network_to_export = "fakeFile.json";
    manager.app_params.mode = EMode::TrainOnly;
    CHECK(manager.shouldExportNetwork() == true);
    CHECK(!std::filesystem::exists(manager.app_params.network_to_export));
    manager.app_params.network_to_import = "../data/testModel.json";
    CHECK(std::filesystem::exists(manager.app_params.network_to_import));
    CHECK_NOTHROW(manager.importOrBuildNetwork());
    CHECK_NOTHROW(manager.exportNetwork());
    CHECK(std::filesystem::exists("fakeFile.json"));
    CHECK(std::filesystem::exists("fakeFile.csv"));
    std::filesystem::remove("fakeFile.json");
    std::filesystem::remove("fakeFile.csv");
    CHECK(!std::filesystem::exists("fakeFile.json"));
    CHECK(!std::filesystem::exists("fakeFile.csv"));
  }

  SUBCASE("Test procesTCPClient") {
    manager.app_params.input = EInput::File;
    CHECK_THROWS_WITH_AS(manager.processTCPClient(""), "TCP socket not set.",
                         ManagerException);
  }

  SUBCASE("Test create http server") {
    CHECK(manager.getHttpServer() == nullptr);
    CHECK_THROWS_WITH_AS(manager.createHttpServer(), "TCP socket not set.",
                         ManagerException);
    manager.app_params.input = EInput::Socket;
    manager.app_params.http_port = 8989;
    CHECK_NOTHROW(manager.createHttpServer());
    CHECK(manager.getHttpServer() != nullptr);
    CHECK(manager.getHttpServer()->getServerPort() ==
          manager.app_params.http_port);
    CHECK_NOTHROW(manager.resetHttpServer());
    CHECK(manager.getHttpServer() == nullptr);
    manager.app_params.input = EInput::File;
  }
}