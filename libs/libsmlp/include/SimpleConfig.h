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

  void parseFile() {
    if (config_file.empty()) {
      return;
    }
    std::string path_in_ext = config_file;
    if (std::filesystem::path p(path_in_ext); p.parent_path().empty()) {
      path_in_ext = "./" + path_in_ext;
    }
    std::ifstream file(path_in_ext);
    json json_model;

    if (!file.is_open()) {
      return; // just return as config file is not mandatory
    }

    if (!json::accept(file)) {
      file.close();
      throw SimpleConfigException("JSON parsing error: invalid JSON file:" +
                                  path_in_ext);
    }
    file.seekg(0, std::ifstream::beg);

    try {
      json_model = json::parse(file);

      if (auto lang_file_j = json_model["lang_file"]) {
        lang_file = lang_file_j;
      }

      if (auto data_file_j = json_model["data_file"]) {
        data_file = data_file_j;
      }

      if (auto model_file_j = json_model["model_file"]) {
        model_file = model_file_j;
      }

      file.close();
      isValidConfig = true;
    } catch (const json::parse_error &e) {
      file.close();
      throw SimpleConfigException("JSON parsing error: " +
                                  std::string(e.what()));
    }
  }
};