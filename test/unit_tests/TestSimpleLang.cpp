#include "SimpleLang.h"
#include "doctest.h"
#include "exception/SimpleLangException.h"

TEST_CASE("Testing the SimpleLang class") {
  SUBCASE("Test constructor") {
    CHECK_THROWS_WITH_AS(SimpleLang("test"),
                         "Failed to open file for reading: ./test",
                         SimpleLangException);

    CHECK_NOTHROW(SimpleLang("../../i18n/en.json"));
  }
}