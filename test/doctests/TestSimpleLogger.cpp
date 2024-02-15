#include "Common.h"
#include "Manager.h"
#include "Predict.h"
#include "SimpleLogger.h"
#include "doctest.h"
#include <math.h>
#include <memory>

TEST_CASE("Testing the SimpleLogger class") {
  const auto &logger = SimpleLogger::getInstance();

  float f = 0.1234567890123456789f;
  float g = 0.0000000000000000001f;

  // CHECK THE LOG OUTPUT
  SUBCASE("Test precision") {
    logger.out("before: ", std::round(f), " [", f, "]");
    logger.setPrecision(3);
    logger.out("after: ", std::round(f), " [", f, "]");
    logger.append("after2: ", std::round(f));
    logger.append(" [");
    logger.append(f);
    logger.append("]");
    logger.endl();
  }

  // CHECK THE LOG OUTPUT
  SUBCASE("Test predict precision") {
    Predict predict;
    std::string result;
    auto &app_params = Manager::getInstance().app_params;
    app_params = {.output_at_end = true, .predict_mode = EPredictMode::CSV};
    result = predict.formatResult({1, 2, 3}, {f, g});
    CHECK(result == "1,2,3,0,0");

    app_params = {.predict_mode = EPredictMode::NumberAndRaw};
    result = predict.formatResult({1, 2, 3}, {f, g});
    CHECK(result == "0,0 [0.123,0]");

    app_params = {.predict_mode = EPredictMode::NumberOnly};
    result = predict.formatResult({1, 2, 3}, {f, g});
    CHECK(result == "0,0");

    app_params = {.predict_mode = EPredictMode::RawOnly};
    result = predict.formatResult({1, 2, 3}, {f, g});
    CHECK(result == "0.123,0");
  }
}