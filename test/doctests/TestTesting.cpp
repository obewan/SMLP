#include "Common.h"
#include "CommonModes.h"
#include "DataFileParser.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "TestingFile.h"
#include "TestingStdin.h"
#include "doctest.h"
#include "exception/TestingException.h"
#include <iostream>
#include <memory>
#include <sstream>

using namespace smlp;

TEST_CASE("Testing the Testing class") {
  SUBCASE("Test constructor") {
    CHECK_NOTHROW({
      auto network = std::make_shared<Network>();
      auto fileparser = std::make_shared<DataFileParser>();
      auto testing = new TestingFile();
      testing->setDataParser(fileparser);
      delete testing;
    });
  }
  SUBCASE("Test test function with stdin") {
    // Save the original buffer
    std::streambuf *cin_buffer = std::cin.rdbuf();

    std::string inputData =
        "0, "
        "0.08,0.43,0.90,0.42,1.00,0.62,0.33,0.38,0.10,0.07,0.00,0.00,0.38,0.00,"
        "0.00,1.00,0.92,0.00,1.00,0.00\n"
        "1, 0.01,0.57, 0.90, 0.25, 1.00, 0.00, 0.67, 0.92, 0.09, 0.02, 0.00, "
        "0.00, 0.62,0.00, 0.00, 1.00, 0.92, 0.00, 1.00,0.00\n";
    std::stringstream inputDataStream(inputData);

    auto &manager = Manager::getInstance();
    manager.network =
        std::make_shared<Network>(NetworkParameters{.input_size = 20,
                                                    .hidden_size = 12,
                                                    .output_size = 1,
                                                    .hiddens_count = 1});
    auto fileparser = std::make_shared<DataFileParser>();

    manager.app_params = {.training_ratio_line = 0,
                          .use_training_ratio_line = true,
                          .mode = EMode::TrainTestMonitored,
                          .input = EInput::Stdin};
    auto testing = new TestingStdin();
    testing->setDataParser(fileparser);

    // Redirect std::cin
    std::cin.rdbuf(inputDataStream.rdbuf());

    CHECK_NOTHROW(testing->test(););

    // Restore std::cin to normal after the test
    std::cin.rdbuf(cin_buffer);
  }
  SUBCASE("Test exception") {
    auto &manager = Manager::getInstance();
    manager.app_params.data_file = "../data/test_file.csv";
    auto fileparser = std::make_shared<DataFileParser>();
    manager.app_params.training_ratio = 1;
    manager.app_params.mode = EMode::TrainThenTest;
    auto testing = new TestingFile();
    testing->setDataParser(fileparser);
    CHECK_THROWS_WITH_AS(
        testing->test(),
        SimpleLang::Error(Error::InvalidTrainingRatioTooBig).c_str(),
        TestingException);
    delete testing;
  }
}