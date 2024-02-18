#include "TrainingFile.h"
#include "CommonResult.h"
#include "Manager.h"
#include "SimpleLogger.h"

using namespace smlp;

smlp::Result TrainingFile::train(const std::string &line) {
  if (!fileParser_) {
    throw TrainingException(SimpleLang::Error(Error::InternalError));
  }
  const auto &app_params = Manager::getInstance().app_params;

  if (!fileParser_->isTrainingRatioLineProcessed) {
    fileParser_->calcTrainingRatioLine(app_params);
  }
  if (fileParser_->training_ratio_line == 0) {
    throw TrainingException(
        SimpleLang::Error(Error::InvalidTrainingRatioTooSmall));
  }
  fileParser_->openFile();
  const auto &testing = Manager::getInstance().getTesting();

  const auto &logger = SimpleLogger::getInstance();
  const auto start{std::chrono::steady_clock::now()};
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    logger.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
               app_params.num_epochs, "... ");
    fileParser_->resetPos();
    for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
      processInputLine();
    }
    if (app_params.mode == EMode::TrainTestMonitored) {
      if (!testing) {
        throw TrainingException(SimpleLang::Error(Error::InternalError));
      }
      logger.append("testing... ");
      testing->test("", epoch);
      logger.out(testing->getTestingResults()->getResultsLine());
    } else {
      logger.endl();
    }
  }
  const auto end{std::chrono::steady_clock::now()};
  fileParser_->closeFile();

  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger.info("Elapsed time: ", elapsed_seconds.count(), "s");
  if (app_params.mode == EMode::TrainTestMonitored) {
    if (!testing) {
      throw TrainingException(SimpleLang::Error(Error::InternalError));
    }
    logger.info(testing->getTestingResults()->getResultsDetailled());
  }

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}