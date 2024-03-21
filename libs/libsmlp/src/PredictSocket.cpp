#include "PredictSocket.h"
#include "CommonModes.h"
#include "DataParser.h"
#include "SimpleLogger.h"

using namespace smlp;

Result PredictSocket::predictData(const std::string &line) {
  std::string output;
  const auto &result = parser_.processLine(line);
  if (result.isSuccess) {
    output = processResult(result);
  }
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .action = smlp::getModeStr(EMode::Predict),
          .data = output};
}