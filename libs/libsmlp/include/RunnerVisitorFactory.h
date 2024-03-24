/**
 * @file RunnerVisitorFactory.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief RunnerVisitor Factory
 * @date 2024-03-24
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "RunnerFileVisitor.h"
#include "RunnerSocketVisitor.h"
#include "RunnerStdinVisitor.h"
#include <memory>

namespace smlp {
class RunnerVisitorFactory {
public:
  const RunnerVisitor &getRunnerFileVisitor() {
    if (!runnerFileVisitor_) {
      runnerFileVisitor_ = std::make_unique<RunnerFileVisitor>();
    }
    return *runnerFileVisitor_;
  }
  const RunnerVisitor &getRunnerSocketVisitor() {
    if (!runnerSocketVisitor_) {
      runnerSocketVisitor_ = std::make_unique<RunnerSocketVisitor>();
    }
    return *runnerSocketVisitor_;
  }
  const RunnerVisitor &getRunnerStdinVisitor() {
    if (!runnerStdinVisitor_) {
      runnerStdinVisitor_ = std::make_unique<RunnerStdinVisitor>();
    }
    return *runnerStdinVisitor_;
  }

private:
  std::unique_ptr<RunnerFileVisitor> runnerFileVisitor_ = nullptr;
  std::unique_ptr<RunnerSocketVisitor> runnerSocketVisitor_ = nullptr;
  std::unique_ptr<RunnerStdinVisitor> runnerStdinVisitor_ = nullptr;
};
} // namespace smlp
