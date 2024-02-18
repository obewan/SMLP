#include "TrainingStdin.h"
#include "Manager.h"

using namespace smlp;

smlp::Result TrainingStdin::train(const std::string &line) {
  const auto &logger = SimpleLogger::getInstance();
  logger.log(LogLevel::INFO, false, "Training...");
  size_t current_line = 0;
  std::string lineIn;
  while (std::getline(std::cin, lineIn) &&
         (fileParser_->training_ratio_line == 0 ||
          current_line < fileParser_->training_ratio_line)) {
    processInputLine(lineIn);
    current_line++;
  }
  const auto &manager = Manager::getInstance();
  if (manager.app_params.mode == EMode::TrainTestMonitored) {
    logger.append("testing... ");
    manager.getTesting()->test();
    logger.out(manager.getTesting()->getTestingResults()->getResultsLine());
  }

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success)};
}