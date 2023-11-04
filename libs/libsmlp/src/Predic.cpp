#include "Common.h"
#include "Predict.h"
#include "exception/PredictException.h"
#include <math.h>

void Predict::predict() {
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
  if (!values.empty()) {
    logger_.append(roundValues ? round(values.front()) : values.front());
    for (auto it = std::next(values.begin()); it != values.end(); ++it) {
      logger_.append(",", roundValues ? round(*it) : *it);
    }
  }
}

void Predict::showOutput(const std::vector<float> &inputs,
                         const std::vector<float> &predicteds) const {
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
    throw PredictException("Unimplemented predictive mode");
  }
}