#include "TrainingFile.h"
#include "CommonResult.h"
#include "DataParser.h"
#include "Manager.h"
#include "SimpleLogger.h"

using namespace smlp;

smlp::Result TrainingFile::train(const std::string &line) {
  // Check if dataParser_ is initialized and is of type DataFileParser
  if (!dataParser_ || dataParser_->dataType != DataParserType::DataFileParser) {
    throw TrainingException(SimpleLang::Error(Error::InternalError));
  }

  // Downcast dataParser_ to DataFileParser
  std::shared_ptr<DataFileParser> dataFileParser =
      std::dynamic_pointer_cast<DataFileParser>(dataParser_);

  // Check if downcast was successful
  if (dataFileParser == nullptr) {
    throw TestingException(SimpleLang::Error(Error::InternalError));
  }

  // Get application parameters from the singleton instance of Manager
  const auto &app_params = Manager::getInstance().app_params;

  // Calculate training ratio line if not already done
  if (!dataFileParser->isTrainingRatioLineProcessed) {
    dataFileParser->calcTrainingRatioLine(app_params);
  }

  // If training_ratio_line is 0, throw an exception
  if (dataFileParser->training_ratio_line == 0) {
    throw TrainingException(
        SimpleLang::Error(Error::InvalidTrainingRatioTooSmall));
  }

  // Open the file
  dataFileParser->openFile();

  // Get the testing instance from the singleton instance of Manager
  const auto &testing = Manager::getInstance().getTesting();

  // Get the logger instance
  const auto &logger = SimpleLogger::getInstance();

  // Start the timer
  const auto start{std::chrono::steady_clock::now()};

  // Train for a number of epochs
  for (size_t epoch = 0; epoch < app_params.num_epochs; epoch++) {
    // Log the start of the epoch
    logger.log(LogLevel::INFO, false, "Training epoch ", epoch + 1, "/",
               app_params.num_epochs, "... ");

    // Reset the file position
    dataFileParser->resetPos();

    // Process lines up to the training ratio line
    for (size_t i = 0; i < dataFileParser->training_ratio_line; i++) {
      processInputLine();
    }

    // If mode is TrainTestMonitored, test the network
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

  // Stop the timer
  const auto end{std::chrono::steady_clock::now()};

  // Close the file
  dataFileParser->closeFile();

  // Calculate and log the elapsed time
  const std::chrono::duration<double> elapsed_seconds{end - start};
  logger.info("Elapsed time: ", elapsed_seconds.count(), "s");

  // If mode is TrainTestMonitored, log the detailed testing results
  if (app_params.mode == EMode::TrainTestMonitored) {
    if (!testing) {
      throw TrainingException(SimpleLang::Error(Error::InternalError));
    }
    logger.info(testing->getTestingResults()->getResultsDetailled());
  }

  // Return success
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}
