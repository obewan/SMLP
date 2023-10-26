#include "Training.h"
#include "doctest.h"

TEST_CASE("Testing the Training class") {
  SUBCASE("Test Constructor") { CHECK_NOTHROW(Training(nullptr, "")); }

  // beware current path is "build/libs/libsmlp/test"
  std::string test_file = "../../../../libs/libsmlp/test/test_file.csv";

  Parameters params{.data_file = test_file, .input_size = 20, .output_size = 1};

  auto *network = new Network(params);
  Training training(network, test_file);

  SUBCASE("Test train function") {
    SUBCASE("from_line is greater than to_line") {
      params.from_line = 10;
      params.to_line = 5;
      CHECK_THROWS_AS(training.train(params), TrainingException);
    }

    SUBCASE("from_line is less than to_line") {
      params.from_line = 5;
      params.to_line = 10;
      CHECK_NOTHROW(training.train(params));
    }
  }

  SUBCASE("Test trainAndTest function") {
    SUBCASE("from_line is greater than to_line") {
      params.from_line = 10;
      params.to_line = 5;
      CHECK_THROWS_AS(training.trainAndTest(params), TrainingException);
    }

    SUBCASE("from_line is less than to_line") {
      params.from_line = 5;
      params.to_line = 10;
      CHECK_NOTHROW(training.trainAndTest(params));
    }
  }
}