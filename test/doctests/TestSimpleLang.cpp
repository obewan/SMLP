#include "Common.h"
#include "SimpleLang.h"
#include "doctest.h"
#include "exception/SimpleLangException.h"

TEST_CASE("Testing the SimpleLang class") {
  SUBCASE("Test constructor") {
    CHECK_THROWS_WITH_AS(SimpleLang("test"), "Failed to open file: ./test",
                         SimpleLangException);

    CHECK_NOTHROW(SimpleLang("../../i18n/en.json"));
  }

  SUBCASE("Test parsing") {
    auto lang1 = SimpleLang("../../i18n/en.json");
    std::string s1 = lang1.get("-i,--import_network");
    CHECK(s1.length() > 0);
    std::string s2 = lang1.get("-e,--export_network");
    CHECK(s2.length() > 0);
    CHECK(s1 != s2);
    CHECK(lang1.get(Error::InvalidJsonFile) == "Invalid JSON file");

    auto lang2 = SimpleLang("../../i18n/it.json");
    CHECK(lang2.get(Error::InvalidJsonFile) == "File JSON non valido");
  }
}