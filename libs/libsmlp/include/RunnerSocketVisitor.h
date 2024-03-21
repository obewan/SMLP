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
#include "CommonResult.h"
#include "PredictSocket.h"
#include "RunnerVisitor.h"
#include "TestingSocket.h"
#include "TrainingSocket.h"
#include <memory>

namespace smlp {
class RunnerSocketVisitor : public RunnerVisitor {
public:
  Result visit(const std::string &line = "") const override;

private:
  smlp::Result predict(const std::string &line) const;
  smlp::Result trainOnly(const std::string &line) const;
  smlp::Result testOnly(const std::string &line) const;
  smlp::Result trainTestMonitored(const std::string &line) const;
  smlp::Result trainThenTest(const std::string &line) const;

  mutable std::unique_ptr<PredictSocket> predictSocket_ = nullptr;
  mutable std::unique_ptr<TrainingSocket> trainingSocket_ = nullptr;
  mutable std::unique_ptr<TestingSocket> testingSocket_ = nullptr;
};
} // namespace smlp