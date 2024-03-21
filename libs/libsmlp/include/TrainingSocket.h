/**
 * @file TrainingSocket.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TrainingSocket
 * @date 2023-12-12
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonResult.h"
#include "SimpleLogger.h"
#include "TestingSocket.h"
#include "Training.h"

namespace smlp {
class TrainingSocket : public Training {
public:
  TrainingSocket() : Training(TrainingType::TrainingSocket) {}

  smlp::Result train(const std::string &line = "") override;

  smlp::Result trainTestMonitored(std::unique_ptr<TestingSocket> &testing,
                                  const std::string &line = "");
};
} // namespace smlp