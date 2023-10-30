#include "Training.h"
#include "doctest.h"

TEST_CASE("Testing the Training class") {
  SUBCASE("Test Constructor") { CHECK_NOTHROW(Training(nullptr, "")); }

  // beware current path is "build/libs/libsmlp/test"
  std::string test_file = "../../../../libs/libsmlp/test/data/test_file.csv";

  NetworkParameters params{
      .data_file = test_file, .input_size = 20, .output_size = 1};

  auto network = new Network(params);
  Training training(network, test_file);

  SUBCASE("Test train function") {
    SUBCASE("invalid training_ratio") {
      params.training_ratio = 0;
      CHECK_THROWS_AS(training.train(params), TrainingException);
    }

    SUBCASE("valid training_ratio") {
      params.training_ratio = 0.5f;
      CHECK_NOTHROW(training.train(params));
    }
  }

  SUBCASE("Test trainTestMonitored function") {
    SUBCASE("invalid training_ratio") {
      params.training_ratio = 0;
      CHECK_THROWS_AS(training.trainTestMonitored(params), TrainingException);
    }

    SUBCASE("valid training_ratio") {
      params.training_ratio = 0.5f;
      CHECK_NOTHROW(training.trainTestMonitored(params));
      CHECK(training.getFileParser()->isTrainingRatioLineProcessed == true);
      CHECK(training.getFileParser()->training_ratio_line == 5);
      CHECK(training.getFileParser()->total_lines == 10);
    }

    SUBCASE("valid testing") {
      params.training_ratio = 0.6f;
      params.learning_rate = 0.01f;
      params.hidden_size = 12;
      params.hiddens_count = 1;
      params.num_epochs = 2;
      params.mode = Mode::TrainTestMonitored;
      CHECK_NOTHROW(training.trainTestMonitored(params));

      auto testing = training.getTesting();
      CHECK(testing != nullptr);
      auto testProgress = testing->progress;
      CHECK(testProgress.empty() == false);
      CHECK(testProgress.size() ==
            training.getFileParser()->total_lines -
                training.getFileParser()->training_ratio_line);
      for (auto const &[key, values] : testProgress) {
        CHECK(values.size() == params.num_epochs);
      }

      // Get the first element
      auto it_first = testProgress.begin();
      auto firstKey = it_first->first;
      auto firstValue = it_first->second;
      CHECK(firstKey == training.getFileParser()->training_ratio_line + 1);

      // Get the last element
      auto it_last = --testProgress.end();
      auto lastKey = it_last->first;
      auto lastValue = it_last->second;
      CHECK(lastKey == training.getFileParser()->total_lines);
      CHECK(lastKey > firstKey);
      CHECK(lastValue.back() > firstValue.front());

      auto stats = testing->calcStats();
      CHECK(stats.convergence > 0);
    }
  }
}