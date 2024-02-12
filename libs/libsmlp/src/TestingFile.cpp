#include "TestingFile.h"
#include "Manager.h"

void TestingFile::test(const std::string &line, size_t epoch,
                       size_t current_line_number) {
  testingResults_->clearStats();
  const auto &manager = Manager::getInstance();
  if (!fileParser_ || !manager.network) {
    throw TestingException(SimpleLang::Error(Error::InternalError));
  }
  if (manager.app_params.mode == EMode::TrainThenTest ||
      manager.app_params.use_training_ratio_line) {
    if (!fileParser_->isTrainingRatioLineProcessed) {
      fileParser_->calcTrainingRatioLine(manager.app_params);
    }
    if (fileParser_->training_ratio_line >= fileParser_->total_lines) {
      throw TestingException(
          SimpleLang::Error(Error::InvalidTrainingRatioTooBig));
    }
  }

  if (!fileParser_->file.is_open()) {
    fileParser_->openFile();
  }

  while (true) {
    Common::RecordResult result = fileParser_->processLine();
    if (!result.isSuccess) {
      break;
    }
    auto testResult = testLine(result, fileParser_->current_line_number, epoch);
    testingResults_->processRecordTestingResult(testResult);
  }
}