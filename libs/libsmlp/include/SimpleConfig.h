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
#include "SimpleLogger.h"
#include "exception/SimpleConfigException.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

using json = nlohmann::json;

class SimpleConfig {
public:
  SimpleConfig() = default;
  explicit SimpleConfig(const std::string &filename) : config_file(filename) {
    parseFile();
  };
  std::string config_file = "";
  std::string lang_file = "i18n/en.json";
  std::string data_file = "";
  std::string model_file = "";
  bool isValidConfig = false;

private:
  void parseFile() {
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
      throw SimpleConfigException("JSON parsing error: invalid JSON file:" +
                                  path_in_ext);
    }
    file.seekg(0, std::ifstream::beg);

    parseJson(file);
    file.close();
  }

  void parseJson(std::ifstream &file) {
    json json_model = json::parse(file);

    if (json_model.contains("lang_file")) {
      lang_file = json_model.at("lang_file").get<std::string>();
    }

    if (json_model.contains("data_file")) {
      data_file = json_model.at("data_file").get<std::string>();
    }

    if (json_model.contains("model_file")) {
      model_file = json_model.at("model_file").get<std::string>();
    }

    isValidConfig = true;
  }
};