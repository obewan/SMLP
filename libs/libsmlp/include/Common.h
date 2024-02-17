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

namespace smlp {
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

inline std::string getModeStr(EMode mode) {
  for (const auto &[key, value] : mode_map) {
    if (value == mode) {
      return key;
    }
  }
  return "";
}

inline std::string getPredictModeStr(EPredictMode predict) {
  for (const auto &[key, value] : predict_map) {
    if (value == predict) {
      return key;
    }
  }
  return "";
}

inline std::string getActivationStr(EActivationFunction activation) {
  for (const auto &[key, value] : activation_map) {
    if (value == activation) {
      return key;
    }
  }
  return "";
}

/**
 * @brief Get the escaped string
 *
 * @param st
 * @return std::string
 */
inline std::string getEscapedString(const std::string &st) {
  std::string fullString;
  std::ranges::for_each(st, [&fullString](char c) {
    if (c == '\n') {
      fullString += "\\n";
    } else if (c == '\r') {
      fullString += "\\r";
    } else {
      fullString += c;
    }
  });
  return fullString;
}

// Trim from start
inline std::string ltrim(const std::string &s) {
  std::string trimmed(s);
  trimmed.erase(trimmed.begin(),
                std::find_if(trimmed.begin(), trimmed.end(),
                             [](char ch) { return !std::isspace(ch); }));
  return trimmed;
}

// Trim from end
inline std::string rtrim(const std::string &s) {
  std::string trimmed(s);
  trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
                             [](char ch) { return !std::isspace(ch); })
                    .base(),
                trimmed.end());
  return trimmed;
}

// Trim from both ends
inline std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

// Trim CRLF
inline std::string trimCRLF(const std::string &str) {
  std::string trimmed(str);
  while (!trimmed.empty() &&
         (trimmed.front() == '\r' || trimmed.front() == '\n')) {
    trimmed.erase(trimmed.begin());
  }
  while (!trimmed.empty() &&
         (trimmed.back() == '\r' || trimmed.back() == '\n')) {
    trimmed.pop_back();
  }
  return trimmed;
}

// Right Trim NULL char
// note: there will be no Left Trim NULL function, because a NULL char at the
// beginning of a string invalidate the string, like an end of buffer.
inline std::string trimRNULL(const std::string &str) {
  std::string trimmed(str);
  while (!trimmed.empty() && trimmed.back() == '\0') {
    trimmed.pop_back();
  }
  return trimmed;
}

// Trim spaces + CRLF + NULL
inline std::string trimALL(const std::string &str) {
  return trim(trimCRLF(trimRNULL(str)));
}

} // namespace smlp