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
#include "PredictFile.h"
#include "RunnerVisitor.h"
#include "TestingFile.h"
#include "TrainingFile.h"
#include <memory>

namespace smlp {
class RunnerFileVisitor : public RunnerVisitor {
public:
  Result visit(const std::string &line = "") const override;

private:
  void predict() const;
  void trainOnly() const;
  void testOnly() const;
  void trainTestMonitored() const;
  void trainThenTest() const;

  mutable std::unique_ptr<PredictFile> predictFile_ = nullptr;
  mutable std::unique_ptr<TrainingFile> trainingFile_ = nullptr;
  mutable std::unique_ptr<TestingFile> testingFile_ = nullptr;
};
} // namespace smlp