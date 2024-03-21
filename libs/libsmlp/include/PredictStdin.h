/**
 * @file PredictStdin.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Predict for Stdin (pipe) mode
 * @date 2024-03-21
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "DataParser.h"
#include "Predict.h"

namespace smlp {
class PredictStdin : public Predict {
public:
  Result predictData(const std::string &line = "") override;

private:
  DataParser parser_;
};
} // namespace smlp