#include "TestingStdin.h"
#include "CommonResult.h"
#include "Manager.h"

using namespace smlp;

smlp::Result TestingStdin::test(const std::string &line, size_t epoch,
                                size_t current_line_number) {
  const auto &app_params = Manager::getInstance().app_params;
  if (app_params.mode == EMode::TrainThenTest ||
      app_params.use_training_ratio_line) {
    current_line_number = app_params.training_ratio_line;
  }

  std::string current_line;
  while (std::getline(std::cin, current_line)) {
    dataParser_->current_line_number = current_line_number - 1;
    smlp::RecordResult result = dataParser_->processLine(current_line, true);
    if (!result.isSuccess) {
      continue;
    }
    auto testResult = testLine(result, dataParser_->current_line_number);
    current_line_number++;
    testingResults_->processRecordTestingResult(testResult);
  }

  const auto &finalResult = getTestingResults();
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .data = finalResult->getResultsJson()};
}