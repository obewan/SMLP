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
#include "PredictStdin.h"
#include "RunnerVisitor.h"
#include "TestingStdin.h"
#include "TrainingStdin.h"
#include <memory>

namespace smlp {
class RunnerStdinVisitor : public RunnerVisitor {
public:
  Result visit(const std::string &line = "") const override;

private:
  Result predict() const;
  Result trainOnly() const;
  Result testOnly() const;
  Result trainTestMonitored() const;
  Result trainThenTest() const;

  mutable std::unique_ptr<PredictStdin> predictStdin_ = nullptr;
  mutable std::unique_ptr<TrainingStdin> trainingStdin_ = nullptr;
  mutable std::unique_ptr<TestingStdin> testingStdin_ = nullptr;
};
} // namespace smlp