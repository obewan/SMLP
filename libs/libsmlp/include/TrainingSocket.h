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
#include "SimpleLogger.h"
#include "Training.h"
#include "exception/FileParserException.h"

class TrainingSocket : public Training {
public:
  TrainingSocket() : Training(TrainingType::TrainingSocket) {}

  void train(const std::string &line = "") override {
    try {
      processInputLine(line);
    } catch (FileParserException &fpe) {
      SimpleLogger::getInstance().error("FileParserException at line '", line,
                                        "': ", fpe.what());
    }
  }
};