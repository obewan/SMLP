/**
 * @file PredictFile.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Predict for File mode
 * @date 2024-03-21
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "DataFileParser.h"
#include "Predict.h"

namespace smlp {
class PredictFile : public Predict {
public:
  Result predictData(const std::string &line = "") override;

private:
  DataFileParser fileParser_;
};
} // namespace smlp