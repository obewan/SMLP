#include "Predict.h"
#include "Common.h"
#include "CommonModes.h"
#include "CommonResult.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "exception/PredictException.h"
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>

using namespace smlp;

smlp::Result Predict::predict(const std::string &line) const {
  const auto &app_params = Manager::getInstance().app_params;
  switch (app_params.input) {
  case EInput::File:
  case EInput::Stdin:
  case EInput::Socket:
    return processInput(app_params.input, line);
  default:
    return {.code = smlp::make_error_code(smlp::ErrorCode::NotImplemented)};
  }
}

smlp::Result Predict::processInput(EInput input,
                                   const std::string &line) const {
  std::string output;
  bool isParsing = true;
  std::string linein;
  while (isParsing) {
    RecordResult result;
    switch (input) {
    case EInput::File:
      if (!fileParser_->file.is_open()) {
        fileParser_->openFile();
      }
      result = fileParser_->processLine();
      isParsing = result.isSuccess && !result.isEndOfFile;
      break;
    case EInput::Stdin:
      isParsing = static_cast<bool>(std::getline(std::cin, linein));
      if (isParsing) {
        result = fileParser_->processLine(linein);
      }
      break;
    case EInput::Socket:
      result = fileParser_->processLine(line);
      isParsing = false;
      break;
    default:
      break;
    }
    if (result.isSuccess) {
      output = processResult(result);
    }
  }
  if (input == EInput::File) {
    fileParser_->closeFile();
  }
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .message = output};
}

std::string Predict::processResult(const RecordResult &result) const {
  const auto &network = Manager::getInstance().network;
  auto predicteds = network->forwardPropagation(result.record.inputs);
  auto output = formatResult(result.record.inputs, predicteds);
  SimpleLogger::LOG_INFO(output);
  return output;
}

std::string Predict::formatValues(const std::vector<float> &values,
                                  bool roundValues) const {
  // Truncate to zero if close to zero, to avoid scientific notation.
  auto truncZero = [](const float &value) {
    return value < 1e-4 ? 0.0f : value;
  };

  if (!values.empty()) {
    std::stringstream sstr;
    sstr << std::setprecision(3);
    sstr << (roundValues ? round(values.front()) : truncZero(values.front()));
    for (auto it = std::next(values.begin()); it != values.end(); ++it) {
      sstr << "," << (roundValues ? round(*it) : truncZero(*it));
    }
    return sstr.str();
  }
  return "";
}

std::string Predict::formatResult(const std::vector<float> &inputs,
                                  const std::vector<float> &predicteds) const {
  const auto &app_params = Manager::getInstance().app_params;
  std::stringstream sstr;
  sstr << std::setprecision(3);

  switch (app_params.predictive_mode) {
  case EPredictiveMode::CSV:
    sstr << formatValues(app_params.output_at_end ? inputs : predicteds,
                         !app_params.output_at_end)
         << ","
         << formatValues(app_params.output_at_end ? predicteds : inputs,
                         app_params.output_at_end);
    break;
  case EPredictiveMode::NumberAndRaw:
    sstr << formatValues(predicteds, true) << " ["
         << formatValues(predicteds, false) << "]";
    break;
  case EPredictiveMode::NumberOnly:
    sstr << formatValues(predicteds, true);
    break;
  case EPredictiveMode::RawOnly:
    sstr << formatValues(predicteds, false);
    break;
  default:
    throw PredictException(
        SimpleLang::Error(Error::UnimplementedPredictiveMode));
  }
  return sstr.str();
}