#include "Common.h"
#include "SimpleLang.h"
#include "doctest.h"
#include "exception/SimpleLangException.h"

TEST_CASE("Testing the SimpleLang class") {
  SUBCASE("Test constructor") {
    CHECK_THROWS_WITH_AS(SimpleLang::getInstance().parseFile("test"),
                         "Failed to open file: ./test", SimpleLangException);

    CHECK_NOTHROW(SimpleLang::getInstance().parseFile("../../i18n/en.json"));
  }

  SUBCASE("Test parsing") {
    const auto &lang = SimpleLang::getInstance();
    lang.parseFile("../../i18n/en.json");
    std::string s1 = lang.get("-i,--import_network");
    CHECK(s1.length() > 0);
    std::string s2 = lang.get("-e,--export_network");
    CHECK(s2.length() > 0);
    CHECK(s1 != s2);
    CHECK(lang.get(Error::InvalidJsonFile) == "Invalid JSON file");
    CHECK(lang.get(Error::DifferentModelVersion,
                   {{"vuser", "0.1"}, {"vcurrent", "0.2"}}) ==
          "Your file model version (0.1) is not the same as current "
          "version (0.2)");

    lang.parseFile("../../i18n/it.json");
    CHECK(lang.get(Error::InvalidJsonFile) == "File JSON non valido");

    lang.parseFile("../../i18n/de.json");
    CHECK(lang.get(Error::InvalidJsonFile) == "Ungültige JSON-Datei");

    lang.parseFile("../../i18n/es.json");
    CHECK(lang.get(Error::InvalidJsonFile) == "Archivo JSON no válido");

    lang.parseFile("../../i18n/fr.json");
    CHECK(lang.get(Error::InvalidJsonFile) == "Fichier JSON invalide");

    lang.parseFile("../../i18n/pt.json");
    CHECK(lang.get(Error::InvalidJsonFile) == "Arquivo JSON inválido");
    CHECK(lang.get(Error::InvalidTotalInput) ==
          "Número inválido de valores de entrada");
  }
}