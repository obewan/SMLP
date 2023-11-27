#include "Common.h"
#include "Predict.h"
#include "SimpleLogger.h"
#include "doctest.h"
#include <math.h>
#include <memory>

TEST_CASE("Testing the SimpleLogger class") {
  auto logger = std::make_shared<SimpleLogger>();
  const Predict predict(
      nullptr, {.predictive_mode = EPredictiveMode::NumberAndRaw}, logger);
  float f = 0.1234567890123456789f;
  float g = 0.0000000000000000001f;

  // CHECK THE LOG OUTPUT
  SUBCASE("Test precision") {
    logger->out("before: ", std::round(f), " [", f, "]");
    logger->setPrecision(3);
    logger->out("after: ", std::round(f), " [", f, "]");
    logger->append("after2: ", std::round(f));
    logger->append(" [");
    logger->append(f);
    logger->append("]");
    logger->endl();
  }

  // CHECK THE LOG OUTPUT
  SUBCASE("Test predict precision") {
    const Predict predict1(
        nullptr,
        {.output_at_end = true, .predictive_mode = EPredictiveMode::CSV},
        logger);
    const Predict predict2(
        nullptr, {.predictive_mode = EPredictiveMode::NumberAndRaw}, logger);
    const Predict predict3(
        nullptr, {.predictive_mode = EPredictiveMode::NumberOnly}, logger);
    const Predict predict4(
        nullptr, {.predictive_mode = EPredictiveMode::RawOnly}, logger);
    predict1.showOutput({1, 2, 3}, {f, g});
    predict2.showOutput({1, 2, 3}, {f, g});
    predict3.showOutput({1, 2, 3}, {f, g});
    predict4.showOutput({1, 2, 3}, {f, g});
  }
}