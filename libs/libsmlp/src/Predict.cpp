#include "Predict.h"
#include "Common.h"
#include "SimpleLogger.h"
#include "exception/PredictException.h"
#include <iostream>
#include <math.h>

void Predict::predict(const std::string &line) const {
  if (app_params_.use_socket) {
    processLine(line);
  } else if (app_params_.use_stdin) {
    processStdin();
  } else {
    processFile();
  }
}

void Predict::processStdin() const {
  std::string line;
  while (std::getline(std::cin, line)) {
    processLine(line);
  }
}

void Predict::processFile() const {
  if (!fileParser_->file.is_open()) {
    fileParser_->openFile();
  }
  bool isParsing = true;
  while (isParsing) {
    auto result = processLine();
    isParsing = result.isSuccess && !result.isEndOfFile;
  }
  fileParser_->closeFile();
}

RecordResult Predict::processLine(const std::string &line) const {
  RecordResult result =
      fileParser_->processLine(network_->params, app_params_, line);
  if (result.isSuccess) {
    auto predicteds = network_->forwardPropagation(result.record.inputs);
    showOutput(result.record.inputs, predicteds);
  }
  return result;
}

void Predict::appendValues(const std::vector<float> &values,
                           bool roundValues) const {
  // Truncate to zero if close to zero, to avoid scientific notation.
  auto truncZero = [](const float &value) {
    return value < 1e-4 ? 0.0f : value;
  };

  if (!values.empty()) {
    const auto &logger = SimpleLogger::getInstance();
    logger.append(roundValues ? round(values.front())
                              : truncZero(values.front()));
    for (auto it = std::next(values.begin()); it != values.end(); ++it) {
      logger.append(",", roundValues ? round(*it) : truncZero(*it));
    }
  }
}

void Predict::showOutput(const std::vector<float> &inputs,
                         const std::vector<float> &predicteds) const {
  const auto &logger = SimpleLogger::getInstance();
  logger.setPrecision(3);
  switch (app_params_.predictive_mode) {
  case EPredictiveMode::CSV: {
    appendValues(app_params_.output_at_end ? inputs : predicteds,
                 !app_params_.output_at_end);
    logger.append(",");
    appendValues(app_params_.output_at_end ? predicteds : inputs,
                 app_params_.output_at_end);
    logger.endl();
  } break;
  case EPredictiveMode::NumberAndRaw: {
    appendValues(predicteds, true);
    logger.append(" [");
    appendValues(predicteds, false);
    logger.out("]");
  } break;
  case EPredictiveMode::NumberOnly: {
    appendValues(predicteds, true);
    logger.endl();
  } break;
  case EPredictiveMode::RawOnly: {
    appendValues(predicteds, false);
    logger.endl();
  } break;
  default:
    logger.resetPrecision();
    throw PredictException(
        SimpleLang::Error(Error::UnimplementedPredictiveMode));
  }
  logger.resetPrecision();
}