#include "TrainingFile.h"
#include "Manager.h"

void TrainingFile::train(const std::string &line) {
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

  const auto start{std::chrono::steady_clock::now()};
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    logger_.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
                app_params.num_epochs, "... ");
    fileParser_->resetPos();
    for (size_t i = 0; i < fileParser_->training_ratio_line; i++) {
      processInputLine();
    }
    if (app_params.mode == EMode::TrainTestMonitored) {
      if (!testing) {
        throw TrainingException(SimpleLang::Error(Error::InternalError));
      }
      logger_.append("testing... ");
      testing->test("", epoch);
      logger_.out(testing->getTestingResults()->showResultsLine());
    } else {
      logger_.endl();
    }
  }
  const auto end{std::chrono::steady_clock::now()};
  fileParser_->closeFile();

  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger_.info("Elapsed time: ", elapsed_seconds.count(), "s");
  if (app_params.mode == EMode::TrainTestMonitored) {
    if (!testing) {
      throw TrainingException(SimpleLang::Error(Error::InternalError));
    }
    logger_.info(testing->getTestingResults()->showDetailledResults());
  }
}