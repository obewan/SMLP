#include "SimpleConfig.h"
#include "doctest.h"

TEST_CASE("Testing the SimpleConf class") {
  SUBCASE("Test constructor") { CHECK_NOTHROW(SimpleConfig("test")); }
}