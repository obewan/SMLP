#include "SimpleConfig.h"
#include "doctest.h"

TEST_CASE("Testing the SimpleConf class") {
  SUBCASE("Test constructor") { CHECK_NOTHROW(SimpleConfig("test")); }

  SUBCASE("Test parsing") {
    CHECK_NOTHROW(SimpleConfig("data/test_config_empty.json"));
    auto conf = SimpleConfig("data/test_config_empty.json");
    CHECK(conf.config_file == "data/test_config_empty.json");
    CHECK(conf.lang_file == "i18n/en.json");
    CHECK(conf.file_input.empty());
    CHECK(conf.import_network.empty());
    CHECK(conf.isValidConfig);

    auto conf2 = SimpleConfig("data/test_config_lang.json");
    CHECK(conf2.config_file == "data/test_config_lang.json");
    CHECK(conf2.lang_file == "i18n/fr.json");
    CHECK(conf2.file_input.empty());
    CHECK(conf2.import_network.empty());
    CHECK(conf2.export_network.empty());
    CHECK(conf2.isValidConfig);

    auto conf3 = SimpleConfig("data/test_config_full.json");
    CHECK(conf3.config_file == "data/test_config_full.json");
    CHECK(conf3.lang_file == "i18n/fr.json");
    CHECK(conf3.file_input == "test.csv");
    CHECK(conf3.import_network == "model.json");
    CHECK(conf3.export_network == "model_new.json");
    CHECK(conf3.isValidConfig);
  }
}