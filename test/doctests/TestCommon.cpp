#include "Common.h"
#include "doctest.h"

TEST_CASE("Testing the Common class") {

  SUBCASE("Test getStrings") {
    for (auto mode :
         {EMode::TestOnly, EMode::TrainOnly, EMode::TrainTestMonitored,
          EMode::TrainThenTest, EMode::Predictive}) {
      CHECK(!smlp::GetModeStr(mode).empty());
    }

    for (auto pred : {EPredictiveMode::CSV, EPredictiveMode::NumberAndRaw,
                      EPredictiveMode::NumberOnly, EPredictiveMode::RawOnly}) {
      CHECK(!smlp::GetPredictiveModeStr(pred).empty());
    }

    for (auto activ :
         {EActivationFunction::ELU, EActivationFunction::LReLU,
          EActivationFunction::PReLU, EActivationFunction::ReLU,
          EActivationFunction::Sigmoid, EActivationFunction::Tanh}) {
      CHECK(!smlp::GetActivationStr(activ).empty());
    }
  }

  SUBCASE("Test trims") {
    CHECK(smlp::ltrim("  test  ") == "test  ");
    CHECK(smlp::rtrim("  test  ") == "  test");
    CHECK(smlp::trim("  test  ") == "test");
    CHECK(smlp::trimCRLF("\r\ntest") == "test");
    CHECK(smlp::trimCRLF("\r\ntest\r\n") == "test");
    CHECK(smlp::trimCRLF("test\r\n") == "test");
  }
}