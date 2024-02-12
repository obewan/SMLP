/**
 * @file Common.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common tools and objects
 * @date 2023-10-22
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonErrors.h"
#include "CommonMessages.h"
#include "CommonModes.h"
#include "CommonParameters.h"
#include "CommonResult.h"
#include <ctime>
#include <map>
#include <string>
#include <vector>

consteval unsigned long long operator"" _K(unsigned long long x) {
  return x * 1024;
}

consteval unsigned long long operator"" _M(unsigned long long x) {
  return x * 1024_K;
}

consteval unsigned long long operator"" _G(unsigned long long x) {
  return x * 1024_M;
}

namespace Common {
/**
 * @brief Record of inputs vector and outputs vector
 *
 */
struct Record {
  std::vector<float> inputs;
  std::vector<float> outputs;
};

/**
 * @brief Results of a parsed record.
 */
struct RecordResult {
  bool isSuccess = false;
  bool isLineSkipped = false;
  bool isEndOfFile = false;
  bool isEndOfTrainingLines = false;
  Record record;
};

inline std::string GetModeStr(EMode mode) {
  for (const auto &[key, value] : mode_map) {
    if (value == mode) {
      return key;
    }
  }
  return "";
}

inline std::string GetPredictiveModeStr(EPredictiveMode predictive) {
  for (const auto &[key, value] : predictive_map) {
    if (value == predictive) {
      return key;
    }
  }
  return "";
}

inline std::string GetActivationStr(EActivationFunction activation) {
  for (const auto &[key, value] : activation_map) {
    if (value == activation) {
      return key;
    }
  }
  return "";
}

// Trim from start (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](char ch) { return !std::isspace(ch); }));
}

// Trim from end (in place)
inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// Trim from both ends (in place)
inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}
} // namespace Common