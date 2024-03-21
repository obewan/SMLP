#include "TrainingStdin.h"
#include "Manager.h"
#include "TestingStdin.h"

using namespace smlp;

smlp::Result TrainingStdin::train(const std::string &line) {
  const auto &logger = SimpleLogger::getInstance();
  logger.log(LogLevel::INFO, false, "Training...");
  size_t current_line = 0;
  std::string lineIn;

  // Process lines from standard input until there are no more lines or until
  // the training ratio line is reached
  while (std::getline(std::cin, lineIn) &&
         (dataParser_->training_ratio_line == 0 ||
          current_line < dataParser_->training_ratio_line)) {
    processInputLine(lineIn);
    current_line++;
  }

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}

smlp::Result
TrainingStdin::trainTestMonitored(std::unique_ptr<TestingStdin> &testing,
                                  const std::string &line) {
  const auto &logger = SimpleLogger::getInstance();
  logger.log(LogLevel::INFO, false, "Training...");
  size_t current_line = 0;
  std::string lineIn;

  // Process lines from standard input until there are no more lines or until
  // the training ratio line is reached
  while (std::getline(std::cin, lineIn) &&
         (dataParser_->training_ratio_line == 0 ||
          current_line < dataParser_->training_ratio_line)) {
    processInputLine(lineIn);
    current_line++;
  }

  logger.append("testing... ");
  testing->test();
  logger.out(testing->getTestingResults()->getResultsLine());

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}
