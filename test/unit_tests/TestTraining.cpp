#include "Common.h"
#include "SimpleLogger.h"
#include "Training.h"
#include "doctest.h"
#include <memory>

TEST_CASE("Testing the Training class") {
  SimpleLogger logger;
  SUBCASE("Test Constructor") { CHECK_NOTHROW(Training(nullptr, "", logger)); }

  std::string test_file = "../data/test_file.csv";

  NetworkParameters network_params{.input_size = 20, .output_size = 1};
  AppParameters app_params{.data_file = test_file};

  auto network = std::make_shared<Network>(network_params);
  Training training(network, test_file, logger);

  SUBCASE("Test train function") {
    SUBCASE("invalid training_ratio") {
      app_params.training_ratio = 0;
      CHECK_THROWS_AS(training.train(network_params, app_params),
                      TrainingException);
    }

    SUBCASE("valid training_ratio") {
      app_params.training_ratio = 0.5f;
      CHECK_NOTHROW(training.train(network_params, app_params));
    }
  }

  SUBCASE("Test trainTestMonitored function") {
    SUBCASE("invalid training_ratio") {
      app_params.training_ratio = 0;
      CHECK_THROWS_AS(training.trainTestMonitored(network_params, app_params),
                      TrainingException);
    }

    SUBCASE("valid training_ratio") {
      app_params.training_ratio = 0.5f;
      CHECK_NOTHROW(training.trainTestMonitored(network_params, app_params));
      CHECK(training.getFileParser()->isTrainingRatioLineProcessed == true);
      CHECK(training.getFileParser()->training_ratio_line == 5);
      CHECK(training.getFileParser()->total_lines == 10);
    }

    SUBCASE("valid testing") {
      network_params.learning_rate = 0.01f;
      network_params.hidden_size = 12;
      network_params.hiddens_count = 1;
      app_params.training_ratio = 0.6f;
      app_params.num_epochs = 2;
      app_params.mode = EMode::TrainTestMonitored;
      CHECK_NOTHROW(training.trainTestMonitored(network_params, app_params));

      auto testing = training.getTesting();
      CHECK(testing != nullptr);
      auto testProgress = testing->progress;
      CHECK(testProgress.empty() == false);
      CHECK(testProgress.size() ==
            training.getFileParser()->total_lines -
                training.getFileParser()->training_ratio_line);
      for (auto const &[key, values] : testProgress) {
        CHECK(values.size() == app_params.num_epochs);
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

      auto stats = testing->calcStats(true);
      CHECK(stats.convergence > 0);
    }
  }
}