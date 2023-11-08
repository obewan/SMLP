#include "Common.h"
#include "doctest.h"

TEST_CASE("Testing the Common class") {

  SUBCASE("Test getStrings") {
    for (auto mode :
         {EMode::TestOnly, EMode::TrainOnly, EMode::TrainTestMonitored,
          EMode::TrainThenTest, EMode::Predictive}) {
      CHECK(!Common::getModeStr(mode).empty());
    }

    for (auto pred : {EPredictiveMode::CSV, EPredictiveMode::NumberAndRaw,
                      EPredictiveMode::NumberOnly, EPredictiveMode::RawOnly}) {
      CHECK(!Common::getPredictiveModeStr(pred).empty());
    }

    for (auto activ :
         {EActivationFunction::ELU, EActivationFunction::LReLU,
          EActivationFunction::PReLU, EActivationFunction::ReLU,
          EActivationFunction::Sigmoid, EActivationFunction::Tanh}) {
      CHECK(!Common::getActivationStr(activ).empty());
    }
  }
}