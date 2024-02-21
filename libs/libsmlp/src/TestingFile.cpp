#include "TestingFile.h"
#include "Manager.h"
#include "exception/TestingFileException.h"

using namespace smlp;

smlp::Result TestingFile::test(const std::string &line, size_t epoch,
                               size_t current_line_number) {
  // Clear previous testing results
  testingResults_->clearStats();

  // Check the manager neural network
  const auto &manager = Manager::getInstance();
  if (!manager.network) {
    throw TestingFileException(SimpleLang::Error(Error::TestingError));
  }

  // Downcast dataParser_ to DataFileParser
  if (!dataParser_ || dataParser_->dataType != DataParserType::DataFileParser) {
    throw TestingFileException(
        SimpleLang::Error(Error::DataFileParserInstanceError));
  }
  std::shared_ptr<DataFileParser> dataFileParser =
      std::dynamic_pointer_cast<DataFileParser>(dataParser_);
  if (dataFileParser == nullptr) {
    throw TestingFileException(
        SimpleLang::Error(Error::DataFileParserDowncastError));
  }

  // If mode is TrainThenTest or use_training_ratio_line is true, calculate
  // training ratio line if not already done
  if (manager.app_params.mode == EMode::TrainThenTest ||
      manager.app_params.use_training_ratio_line) {
    if (!dataFileParser->isTrainingRatioLineProcessed) {
      dataFileParser->calcTrainingRatioLine(manager.app_params);
    }
    // If training_ratio_line is greater than total_lines, throw an exception
    if (dataFileParser->training_ratio_line >= dataFileParser->total_lines) {
      throw TestingFileException(
          SimpleLang::Error(Error::InvalidTrainingRatioTooBig));
    }
  }

  // Open the file if not already open
  if (!dataFileParser->file.is_open()) {
    dataFileParser->openFile();
  }

  // Process lines from the file until a line fails to process
  while (true) {
    smlp::RecordResult result = dataFileParser->processLine("", true);
    if (!result.isSuccess) {
      break;
    }
    // Test the processed line and update testing results
    auto testResult = testLine(result, dataParser_->current_line_number, epoch);
    testingResults_->processRecordTestingResult(testResult);
  }

  // Get the final testing results
  const auto &finalResult = getTestingResults();

  // Return the final results
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .data = finalResult->getResultsJson()};
}