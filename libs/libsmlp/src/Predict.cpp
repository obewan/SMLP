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

std::string Predict::processResult(const RecordResult &result) const {
  const auto &network = Manager::getInstance().network;
  auto predicteds = network->forwardPropagation(result.record.inputs);
  auto output = formatResult(result.record.inputs, predicteds);
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

  switch (app_params.predict_mode) {
  case EPredictMode::CSV:
    sstr << formatValues(app_params.output_at_end ? inputs : predicteds,
                         !app_params.output_at_end)
         << ","
         << formatValues(app_params.output_at_end ? predicteds : inputs,
                         app_params.output_at_end);
    break;
  case EPredictMode::NumberAndRaw:
    sstr << formatValues(predicteds, true) << " ["
         << formatValues(predicteds, false) << "]";
    break;
  case EPredictMode::NumberOnly:
    sstr << formatValues(predicteds, true);
    break;
  case EPredictMode::RawOnly:
    sstr << formatValues(predicteds, false);
    break;
  default:
    throw PredictException(SimpleLang::Error(Error::UnimplementedPredictMode));
  }
  return sstr.str();
}