#include "SimpleConfig.h"
#include "doctest.h"

TEST_CASE("Testing the SimpleConf class") {
  SUBCASE("Test constructor") { CHECK_NOTHROW(SimpleConfig("test")); }

  SUBCASE("Test parsing") {
    CHECK_NOTHROW(SimpleConfig("data/test_config_empty.json"));
    auto conf = SimpleConfig("data/test_config_empty.json");
    CHECK(conf.config_file == "data/test_config_empty.json");
    CHECK(conf.lang_file == "i18n/en.json");
    CHECK(conf.data_file.empty() == true);
    CHECK(conf.model_file.empty() == true);
    CHECK(conf.isValidConfig == true);

    auto conf2 = SimpleConfig("data/test_config_lang.json");
    CHECK(conf2.config_file == "data/test_config_lang.json");
    CHECK(conf2.lang_file == "i18n/fr.json");
    CHECK(conf2.data_file.empty() == true);
    CHECK(conf2.model_file.empty() == true);
    CHECK(conf2.isValidConfig == true);

    auto conf3 = SimpleConfig("data/test_config_full.json");
    CHECK(conf3.config_file == "data/test_config_full.json");
    CHECK(conf3.lang_file == "i18n/fr.json");
    CHECK(conf3.data_file == "test.csv");
    CHECK(conf3.model_file == "model.json");
    CHECK(conf3.isValidConfig == true);
  }
}