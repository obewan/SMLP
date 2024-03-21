#pragma once
#include "TestingStdin.h"
#include "Training.h"

namespace smlp {
class TrainingStdin : public Training {
public:
  TrainingStdin() : Training(TrainingType::TrainingStdin) {}

  smlp::Result train(const std::string &line = "") override;

  smlp::Result trainTestMonitored(std::unique_ptr<TestingStdin> &testing,
                                  const std::string &line = "");
};
} // namespace smlp