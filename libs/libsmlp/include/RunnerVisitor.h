/**
 * @file RunnerVisitor.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief RunnerVisitor interface
 * @date 2024-03-17
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */

#pragma once
#include "CommonResult.h"
#include <string>

namespace smlp {
class RunnerVisitor {
public:
  virtual ~RunnerVisitor() = default;
  /**
   * @brief Runner Visitor
   */
  virtual Result visit(const std::string &line = "") const = 0;
};
} // namespace smlp