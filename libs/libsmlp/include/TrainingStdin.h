#pragma once
#include "Training.h"

class TrainingStdin : public Training {
public:
  TrainingStdin() : Training(TrainingType::TrainingStdin) {}

  Common::Result train(const std::string &line = "") override;
};