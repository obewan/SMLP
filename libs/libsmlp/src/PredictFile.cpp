#include "PredictFile.h"
#include "CommonModes.h"
#include "DataFileParser.h"
#include "SimpleLogger.h"

using namespace smlp;

Result PredictFile::predictData(const std::string &_) {
  DataFileParser fileparser;
  std::string output;
  bool isParsing = true;
  std::string line;
  fileparser.openFile();
  while (isParsing) {
    RecordResult result = fileparser.processLine();
    isParsing = result.isSuccess && !result.isEndOfFile;
    if (result.isSuccess) {
      output = processResult(result);
      SimpleLogger::LOG_INFO(output);
    }
  }
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .action = smlp::getModeStr(EMode::Predict),
          .data = output};
}