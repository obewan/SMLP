/**
 * @file SimpleLanguage.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Simple i18n class
 * @date 2023-11-08
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "../../json/include/json.hpp"
#include "exception/SimpleLanguageException.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

using json = nlohmann::json;

class SimpleLanguage {
public:
  explicit SimpleLanguage(const std::string &filename) { parseFile(filename); };
  std::string get(const std::string &key) {
    auto it = strings.find(key);
    if (it != strings.end()) {
      return it->second;
    } else {
      return "";
    }
  };

private:
  void parseFile(const std::string &filename) {
    std::string path_in_ext = filename;
    if (std::filesystem::path p(path_in_ext); p.parent_path().empty()) {
      path_in_ext = "./" + path_in_ext;
    }
    std::ifstream file(path_in_ext);
    json json_model;

    if (!file.is_open()) {
      throw SimpleLanguageException("Failed to open file for reading: " +
                                    path_in_ext);
    }

    if (!json::accept(file)) {
      file.close();
      throw SimpleLanguageException("JSON parsing error: invalid JSON file:" +
                                    path_in_ext);
    }
    file.seekg(0, std::ifstream::beg);

    try {
      json_model = json::parse(file);

      for (auto it = json_model.begin(); it != json_model.end(); ++it) {
        strings[it.key()] = it.value();
      }

      file.close();
    } catch (const json::parse_error &e) {
      file.close();
      throw SimpleLanguageException("JSON parsing error: " +
                                    std::string(e.what()));
    }
  }
  std::map<std::string, std::string, std::less<>> strings;
};