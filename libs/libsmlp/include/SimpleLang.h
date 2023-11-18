/**
 * @file SimpleLang.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Simple i18n language parser class
 * @date 2023-11-08
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "../../json/include/json.hpp"
#include "exception/SimpleLangException.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

using json = nlohmann::json;

class SimpleLang {
public:
  explicit SimpleLang(const std::string &filename) { parseFile(filename); };

  /**
   * @brief Fetches a localized string for a given key from the parsed JSON
   * file.
   *
   * If variables are provided, the function replaces placeholders in the format
   * of %%variable%% in the string with their corresponding values.
   *
   * @param key The key for the localized string to fetch.
   * @param variables Optional map of variable names and their corresponding
   * replacement values.
   * @return Localized string corresponding to the given key. If the key does
   * not exist, returns an empty string.
   */
  std::string
  get(const std::string &key,
      const std::map<std::string, std::string, std::less<>> &variables = {}) {
    auto it = strings.find(key);
    if (it != strings.end()) {
      std::string str = it->second;
      for (const auto &[varkey, varval] : variables) {
        std::string varKey = "%%" + varkey + "%%";
        size_t start_pos = 0;
        while ((start_pos = str.find(varKey, start_pos)) != std::string::npos) {
          str.replace(start_pos, varKey.length(), varval);
          start_pos += varval.length();
        }
      }
      return str;
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
      throw SimpleLangException("Failed to open file for reading: " +
                                path_in_ext);
    }

    if (!json::accept(file)) {
      file.close();
      throw SimpleLangException("JSON parsing error: invalid JSON file:" +
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
      throw SimpleLangException("JSON parsing error: " + std::string(e.what()));
    }
  }
  std::map<std::string, std::string, std::less<>> strings;
};