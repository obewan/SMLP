/**
 * @file SimpleConfig.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief SimpleConfig class.
 * @date 2023-11-10
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "../../json/include/json.hpp"
#include "SimpleLang.h"
#include "exception/SimpleConfigException.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace smlp {
class SimpleConfig {
public:
  SimpleConfig() = default;
  explicit SimpleConfig(const std::string &filename) : config_file(filename) {
    parseFile();
  };
  std::string config_file = "";
  std::string lang_file = "i18n/en.json";
  std::string file_input = "";
  std::string import_network = "";
  std::string export_network = "";
  bool isValidConfig = false;

private:
  void parseFile() {
    using json = nlohmann::json;
    if (config_file.empty()) {
      return;
    }
    std::string path_in_ext = config_file;
    if (std::filesystem::path p(path_in_ext); p.parent_path().empty()) {
      path_in_ext = "./" + path_in_ext;
    }
    std::ifstream file(path_in_ext);

    if (!file.is_open()) {
      return; // just return as config file is not mandatory
    }

    if (!json::accept(file)) {
      file.close();
      throw SimpleConfigException(SimpleLang::Error(Error::InvalidJsonFile) +
                                  ": " + path_in_ext);
    }
    file.seekg(0, std::ifstream::beg);

    parseJson(file);
    file.close();
  }

  void parseJson(std::ifstream &file) {
    using json = nlohmann::json;
    json json_model = json::parse(file);

    if (json_model.contains("lang_file")) {
      lang_file = json_model.at("lang_file").get<std::string>();
    }

    if (json_model.contains("file_input")) {
      file_input = json_model.at("file_input").get<std::string>();
    }

    if (json_model.contains("import_network")) {
      import_network = json_model.at("import_network").get<std::string>();
    }

    if (json_model.contains("export_network")) {
      export_network = json_model.at("export_network").get<std::string>();
    }

    isValidConfig = true;
  }
};
} // namespace smlp