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
#include "CommonModes.h"
#include "CommonParameters.h"
#include <map>
#include <string>
#include <vector>

/**
 * @brief Record of inputs vector and outputs vector, in that order.
 *
 */
using Record = std::pair<std::vector<float>, std::vector<float>>;

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

/**
 * @brief Common class.
 */
class Common {
public:
  static std::string getModeStr(EMode mode) {
    for (const auto &[key, value] : mode_map) {
      if (value == mode) {
        return key;
      }
    }
    return "";
  }

  static std::string getPredictiveModeStr(EPredictiveMode predictive) {
    for (const auto &[key, value] : predictive_map) {
      if (value == predictive) {
        return key;
      }
    }
    return "";
  }

  static std::string getActivationStr(EActivationFunction activation) {
    for (const auto &[key, value] : activation_map) {
      if (value == activation) {
        return key;
      }
    }
    return "";
  }
};