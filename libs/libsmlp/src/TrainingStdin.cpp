#include "TrainingStdin.h"
#include "Manager.h"

using namespace smlp;

smlp::Result TrainingStdin::train(const std::string &line) {
  // Get the logger instance
  const auto &logger = SimpleLogger::getInstance();

  // Log the start of training
  logger.log(LogLevel::INFO, false, "Training...");

  // Initialize the current line number
  size_t current_line = 0;

  // Initialize a string to hold the current line
  std::string lineIn;

  // Process lines from standard input until there are no more lines or until
  // the training ratio line is reached
  while (std::getline(std::cin, lineIn) &&
         (dataParser_->training_ratio_line == 0 ||
          current_line < dataParser_->training_ratio_line)) {
    // Process the current line
    processInputLine(lineIn);

    // Increment the line number
    current_line++;
  }

  // Get the singleton instance of Manager
  const auto &manager = Manager::getInstance();

  // If mode is TrainTestMonitored, test the network
  if (manager.app_params.mode == EMode::TrainTestMonitored) {
    // Log the start of testing
    logger.append("testing... ");

    // Test the network
    manager.getTesting()->test();

    // Log the testing results
    logger.out(manager.getTesting()->getTestingResults()->getResultsLine());
  }

  // Return success
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}
