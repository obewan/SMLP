/**
 * @file Predict.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Predict class
 * @date 2023-11-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "CommonResult.h"

namespace smlp {
/**
 * @brief Predict class for predictive operations.
 */
class Predict {
public:
  virtual ~Predict() = default;

  virtual Result predictData(const std::string &line = "") = 0;

  std::string formatValues(const std::vector<float> &values,
                           bool roundValues) const;

  std::string formatResult(const std::vector<float> &inputs,
                           const std::vector<float> &predicteds) const;

  /**
   * @brief Process Result, calling forward propagation and formatting output
   *
   * @param result
   * @return std::string
   */
  std::string processResult(const smlp::RecordResult &result) const;
};
} // namespace smlp