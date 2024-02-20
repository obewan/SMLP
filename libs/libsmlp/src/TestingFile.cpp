#include "TestingFile.h"
#include "CommonResult.h"
#include "DataParser.h"
#include "Manager.h"

using namespace smlp;

smlp::Result TestingFile::test(const std::string &line, size_t epoch,
                               size_t current_line_number) {
  testingResults_->clearStats();
  const auto &manager = Manager::getInstance();
  if (!dataParser_ || dataParser_->dataType != DataParserType::DataFileParser ||
      !manager.network) {
    throw TestingException(SimpleLang::Error(Error::InternalError));
  }

  std::shared_ptr<DataFileParser> dataFileParser =
      std::dynamic_pointer_cast<DataFileParser>(dataParser_);
  if (dataFileParser == nullptr) {
    throw TestingException(SimpleLang::Error(Error::InternalError));
  }

  if (manager.app_params.mode == EMode::TrainThenTest ||
      manager.app_params.use_training_ratio_line) {
    if (!dataFileParser->isTrainingRatioLineProcessed) {
      dataFileParser->calcTrainingRatioLine(manager.app_params);
    }
    if (dataFileParser->training_ratio_line >= dataFileParser->total_lines) {
      throw TestingException(
          SimpleLang::Error(Error::InvalidTrainingRatioTooBig));
    }
  }

  if (!dataFileParser->file.is_open()) {
    dataFileParser->openFile();
  }

  while (true) {
    smlp::RecordResult result = dataFileParser->processLine("", true);
    if (!result.isSuccess) {
      break;
    }
    auto testResult = testLine(result, dataParser_->current_line_number, epoch);
    testingResults_->processRecordTestingResult(testResult);
  }

  const auto &finalResult = getTestingResults();
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .data = finalResult->getResultsJson()};
}