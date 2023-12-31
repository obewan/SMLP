#include "TrainingStdin.h"
#include "Manager.h"

void TrainingStdin::train(const std::string &line) {
  logger_.log(LogLevel::INFO, false, "Training...");
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
    logger_.append("testing... ");
    manager.getTesting()->test();
    logger_.out(manager.getTesting()->getTestingResults()->showResultsLine());
  }
}