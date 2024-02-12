#include "TestingStdin.h"
#include "Manager.h"

void TestingStdin::test(const std::string &line, size_t epoch,
                        size_t current_line_number) {
  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.mode == EMode::TrainThenTest ||
      app_params.use_training_ratio_line) {
    current_line_number = app_params.training_ratio_line;
  }

  std::string current_line;
  while (std::getline(std::cin, current_line)) {
    fileParser_->current_line_number = current_line_number - 1;
    Common::RecordResult result = fileParser_->processLine(current_line);
    if (!result.isSuccess) {
      continue;
    }
    auto testResult = testLine(result, fileParser_->current_line_number);
    current_line_number++;
    testingResults_->processRecordTestingResult(testResult);
  }
}