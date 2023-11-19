#include "Predict.h"
#include "Common.h"
#include "exception/PredictException.h"
#include <math.h>

void Predict::predict() const {
  if (app_params_.use_stdin) {
    processStdin();
  } else {
    processFile();
  }
}

void Predict::processStdin() const {
  std::string line;
  while (std::getline(std::cin, line)) {
    RecordResult result =
        fileParser_->processLine(network_->params, app_params_, line);
    if (result.isSuccess) {
      auto predicteds = network_->forwardPropagation(result.record.first);
      showOutput(result.record.first, predicteds);
    }
  }
}

void Predict::processFile() const {
  if (!fileParser_->file.is_open()) {
    fileParser_->openFile();
  }
  bool isParsing = true;
  while (isParsing) {
    RecordResult result =
        fileParser_->processLine(network_->params, app_params_);
    if (result.isSuccess) {
      auto predicteds = network_->forwardPropagation(result.record.first);
      showOutput(result.record.first, predicteds);
    }
    isParsing = result.isSuccess && !result.isEndOfFile;
  }
  fileParser_->closeFile();
}

void Predict::appendValues(const std::vector<float> &values,
                           bool roundValues) const {
  // Truncate to zero if close to zero, to avoid scientific notation.
  auto truncZero = [](const float &value) {
    return value < 1e-4 ? 0.0f : value;
  };

  if (!values.empty()) {
    logger_.append(roundValues ? round(values.front())
                               : truncZero(values.front()));
    for (auto it = std::next(values.begin()); it != values.end(); ++it) {
      logger_.append(",", roundValues ? round(*it) : truncZero(*it));
    }
  }
}

void Predict::showOutput(const std::vector<float> &inputs,
                         const std::vector<float> &predicteds) const {
  logger_.setPrecision(3);
  switch (app_params_.predictive_mode) {
  case EPredictiveMode::CSV: {
    appendValues(app_params_.output_at_end ? inputs : predicteds,
                 !app_params_.output_at_end);
    logger_.append(",");
    appendValues(app_params_.output_at_end ? predicteds : inputs,
                 app_params_.output_at_end);
    logger_.endl();
  } break;
  case EPredictiveMode::NumberAndRaw: {
    appendValues(predicteds, true);
    logger_.append(" [");
    appendValues(predicteds, false);
    logger_.out("]");
  } break;
  case EPredictiveMode::NumberOnly: {
    appendValues(predicteds, true);
    logger_.endl();
  } break;
  case EPredictiveMode::RawOnly: {
    appendValues(predicteds, false);
    logger_.endl();
  } break;
  default:
    logger_.resetPrecision();
    throw PredictException("Unimplemented predictive mode");
  }
  logger_.resetPrecision();
}