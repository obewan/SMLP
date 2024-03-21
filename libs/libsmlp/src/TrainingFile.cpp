#include "TrainingFile.h"
#include "Manager.h"
#include "SimpleLogger.h"
#include "exception/TrainingFileException.h"

using namespace smlp;

smlp::Result TrainingFile::train(const std::string &line) {
  // Check the manager neural network
  const auto &manager = Manager::getInstance();
  if (!manager.network) {
    throw TrainingFileException(SimpleLang::Error(Error::TrainingError));
  }

  // Downcast dataParser_ to DataFileParser
  if (!dataParser_ || dataParser_->dataType != DataParserType::DataFileParser) {
    throw TrainingFileException(
        SimpleLang::Error(Error::DataFileParserInstanceError));
  }
  std::shared_ptr<DataFileParser> dataFileParser =
      std::dynamic_pointer_cast<DataFileParser>(dataParser_);
  if (dataFileParser == nullptr) {
    throw TrainingFileException(
        SimpleLang::Error(Error::DataFileParserDowncastError));
  }

  const auto &app_params = manager.app_params;

  // Calculate training ratio line if not already done
  if (!dataFileParser->isTrainingRatioLineProcessed) {
    dataFileParser->calcTrainingRatioLine(app_params);
  }

  // If training_ratio_line is 0, throw an exception
  if (dataFileParser->training_ratio_line == 0) {
    throw TrainingFileException(
        SimpleLang::Error(Error::InvalidTrainingRatioTooSmall));
  }

  dataFileParser->openFile();
  const auto &logger = SimpleLogger::getInstance();
  const auto start{std::chrono::steady_clock::now()};

  // Train for a number of epochs
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    logger.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
               app_params.num_epochs, "... ");
    dataFileParser->resetPos();
    for (size_t i = 0; i < dataFileParser->training_ratio_line; i++) {
      processInputLine();
    }
    logger.endl();
  }

  const auto end{std::chrono::steady_clock::now()};
  dataFileParser->closeFile();

  // Calculate and log the elapsed time
  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger.info("Elapsed time: ", elapsed_seconds.count(), "s");

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}

smlp::Result
TrainingFile::trainTestMonitored(std::unique_ptr<TestingFile> &testing,
                                 const std::string &line) {

  if (!testing) {
    throw TrainingFileException(SimpleLang::Error(Error::InternalError));
  }
  const auto &manager = Manager::getInstance();
  if (!manager.network) {
    throw TrainingFileException(SimpleLang::Error(Error::TrainingError));
  }

  // Downcast dataParser_ to DataFileParser
  if (!dataParser_ || dataParser_->dataType != DataParserType::DataFileParser) {
    throw TrainingFileException(
        SimpleLang::Error(Error::DataFileParserInstanceError));
  }
  std::shared_ptr<DataFileParser> dataFileParser =
      std::dynamic_pointer_cast<DataFileParser>(dataParser_);
  if (dataFileParser == nullptr) {
    throw TrainingFileException(
        SimpleLang::Error(Error::DataFileParserDowncastError));
  }

  const auto &app_params = manager.app_params;

  // Calculate training ratio line if not already done
  if (!dataFileParser->isTrainingRatioLineProcessed) {
    dataFileParser->calcTrainingRatioLine(app_params);
  }

  // If training_ratio_line is 0, throw an exception
  if (dataFileParser->training_ratio_line == 0) {
    throw TrainingFileException(
        SimpleLang::Error(Error::InvalidTrainingRatioTooSmall));
  }

  dataFileParser->openFile();
  const auto &logger = SimpleLogger::getInstance();
  const auto start{std::chrono::steady_clock::now()};

  // Train for a number of epochs with testing at each epochs
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    logger.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
               app_params.num_epochs, "... ");
    dataFileParser->resetPos();
    for (size_t i = 0; i < dataFileParser->training_ratio_line; i++) {
      processInputLine();
    }
    logger.append("testing... ");
    testing->test("", epoch);
    logger.out(testing->getTestingResults()->getResultsLine());
  }

  const auto end{std::chrono::steady_clock::now()};
  dataFileParser->closeFile();

  // Calculate and log the elapsed time and the testing results
  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger.info("Elapsed time: ", elapsed_seconds.count(), "s");
  logger.info(testing->getTestingResults()->getResultsDetailled());

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}
