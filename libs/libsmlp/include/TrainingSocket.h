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
#include "Training.h"

class TrainingSocket : public Training {
public:
  using Training::Training;
  virtual ~TrainingSocket() = default;

  void train(const std::string &line = "") override { processInputLine(line); }
};