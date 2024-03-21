/**
 * @file TrainingFile.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TrainingFile
 * @date 2023-12-12
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "TestingFile.h"
#include "Training.h"

namespace smlp {
class TrainingFile : public Training {
public:
  TrainingFile() : Training(TrainingType::TrainingFile) {}

  smlp::Result train(const std::string &line = "") override;

  smlp::Result trainTestMonitored(std::unique_ptr<TestingFile> &testing,
                                  const std::string &line = "");
};
} // namespace smlp