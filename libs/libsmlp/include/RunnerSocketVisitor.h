/**
 * @file RunnerSocketVisitor.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief RunnerVisitor for Socket (http) input mode
 * @date 2024-03-21
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */

#pragma once
#include "RunnerVisitor.h"

namespace smlp {
class RunnerSocketVisitor : public RunnerVisitor {
public:
  void visit() const override;
};
} // namespace smlp