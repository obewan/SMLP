#include "PredictStdin.h"
#include "SimpleLogger.h"
#include <iomanip>
#include <iostream>

using namespace smlp;

Result PredictStdin::predictData(const std::string &_) {
  std::string output;
  bool isParsing = true;
  std::string line;

  while (isParsing) {
    RecordResult result;
    isParsing = static_cast<bool>(std::getline(std::cin, line));
    if (isParsing) {
      result = parser_.processLine(line);
    }
    if (result.isSuccess) {
      output = processResult(result);
      SimpleLogger::LOG_INFO(output);
    }
  }
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .action = smlp::getModeStr(EMode::Predict),
          .data = output};
}