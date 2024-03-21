#include "CommonErrors.h"
#include "CommonModes.h"
#include "Manager.h"
#include "RunnerFileVisitor.h"
#include "RunnerSocketVisitor.h"
#include "RunnerStdinVisitor.h"
#include "SimpleLang.h"
#include "Testing.h"
#include "Training.h"
#include "doctest.h"
#include "exception/DataParserException.h"
#include "exception/ManagerException.h"
#include <filesystem>

using namespace smlp;

TEST_CASE("Testing the Manager class") {
  SUBCASE("Test constructor") { CHECK_NOTHROW(Manager::getInstance()); }

  auto &manager = Manager::getInstance();

  SUBCASE("Test file predict") {
    manager.app_params.input = EInput::File;
    manager.app_params.data_file = "";
    manager.app_params.mode = EMode::Predict;

    // Test predict file but without a valid file path.
    CHECK_THROWS_WITH_AS(manager.runWithVisitor(RunnerFileVisitor{});
                         , SimpleLang::Error(Error::FailedToOpenFile).c_str(),
                         DataParserException);

    // Test predict file with a valid file path and network.
    manager.app_params.network_to_import = "../data/testModel.json";
    manager.app_params.data_file = "../data/test_file.csv";
    CHECK(std::filesystem::exists(manager.app_params.network_to_import));
    CHECK(std::filesystem::exists(manager.app_params.data_file));
    CHECK_NOTHROW(manager.importOrBuildNetwork());
    CHECK(manager.network != nullptr);
    CHECK_NOTHROW(manager.runWithVisitor(RunnerFileVisitor{}););
  }

  SUBCASE("Test file training") {
    manager.app_params.mode = EMode::TrainOnly;
    CHECK_NOTHROW(manager.runWithVisitor(RunnerFileVisitor{}));
    manager.app_params.mode = EMode::TrainTestMonitored;
    CHECK_NOTHROW(manager.runWithVisitor(RunnerFileVisitor{}));
  }

  SUBCASE("Test file testing") {
    manager.app_params.mode = EMode::TestOnly;
    CHECK_NOTHROW(manager.runWithVisitor(RunnerFileVisitor{}));
  }

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
    CHECK(manager.http_server == nullptr);
    CHECK_THROWS_WITH_AS(manager.createHttpServer(), "TCP socket not set.",
                         ManagerException);
    manager.app_params.input = EInput::Socket;
    manager.app_params.http_port = 8989;
    CHECK_NOTHROW(manager.createHttpServer());
    CHECK(manager.http_server != nullptr);
    CHECK(manager.http_server->getServerPort() == manager.app_params.http_port);
    CHECK_NOTHROW(manager.resetHttpServer());
    CHECK(manager.http_server == nullptr);
    manager.app_params.input = EInput::File;
  }
}