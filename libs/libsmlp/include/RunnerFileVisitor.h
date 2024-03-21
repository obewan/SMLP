/**
 * @file RunnerFileVisitor.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief RunnerVisitor for File input mode
 * @date 2024-03-21
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */

#pragma once
#include "RunnerVisitor.h"

namespace smlp {
class RunnerFileVisitor : public RunnerVisitor {
public:
  void visit() const override;
};
} // namespace smlp