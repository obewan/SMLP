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
    const auto &lang1 = SimpleLang::getInstance();
    lang1.parseFile("../../i18n/en.json");
    std::string s1 = lang1.get("-i,--import_network");
    CHECK(s1.length() > 0);
    std::string s2 = lang1.get("-e,--export_network");
    CHECK(s2.length() > 0);
    CHECK(s1 != s2);
    CHECK(lang1.get(Error::InvalidJsonFile) == "Invalid JSON file");

    lang1.parseFile("../../i18n/it.json");
    CHECK(lang1.get(Error::InvalidJsonFile) == "File JSON non valido");

    lang1.parseFile("../../i18n/de.json");
    CHECK(lang1.get(Error::InvalidJsonFile) == "Ungültige JSON-Datei");

    lang1.parseFile("../../i18n/es.json");
    CHECK(lang1.get(Error::InvalidJsonFile) == "Archivo JSON no válido");

    lang1.parseFile("../../i18n/fr.json");
    CHECK(lang1.get(Error::InvalidJsonFile) == "Fichier JSON invalide");

    lang1.parseFile("../../i18n/pt.json");
    CHECK(lang1.get(Error::InvalidJsonFile) == "Arquivo JSON inválido");
    CHECK(lang1.get(Error::InvalidTotalInput) ==
          "Número inválido de valores de entrada");
  }
}