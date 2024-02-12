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
#include "Training.h"
#include "exception/FileParserException.h"

class TrainingSocket : public Training {
public:
  TrainingSocket() : Training(TrainingType::TrainingSocket) {}

  Common::Result train(const std::string &line = "") override {
    const auto &processResult = processInputLine(line);
    return {.code = Common::make_error_code(processResult.isSuccess
                                                ? Common::ErrorCode::Success
                                                : Common::ErrorCode::Failure)};
  }
};