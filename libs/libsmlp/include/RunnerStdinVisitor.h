/**
 * @file RunnerStdinVisitor.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief RunnerVisitor for Stdin (pipe) input mode
 * @date 2024-03-21
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */

#pragma once
#include "RunnerVisitor.h"

namespace smlp {
class RunnerStdinVisitor : public RunnerVisitor {
public:
  void visit() const override;
};
} // namespace smlp