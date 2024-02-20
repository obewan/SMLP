#include "TestingStdin.h"
#include "CommonResult.h"
#include "Manager.h"

using namespace smlp;

smlp::Result TestingStdin::test(const std::string &line, size_t epoch,
                                size_t current_line_number) {
  // Get application parameters from the singleton instance of Manager
  const auto &app_params = Manager::getInstance().app_params;

  // If mode is TrainThenTest or use_training_ratio_line is true, set
  // current_line_number to training_ratio_line
  if (app_params.mode == EMode::TrainThenTest ||
      app_params.use_training_ratio_line) {
    current_line_number = app_params.training_ratio_line;
  }

  std::string current_line;
  // Process lines from standard input until there are no more lines
  while (std::getline(std::cin, current_line)) {
    // Set the current line number in dataParser_
    dataParser_->current_line_number = current_line_number - 1;

    // Process the current line
    smlp::RecordResult result = dataParser_->processLine(current_line, true);

    // If the line failed to process, skip to the next line
    if (!result.isSuccess) {
      continue;
    }

    // Test the processed line and increment the line number
    auto testResult = testLine(result, dataParser_->current_line_number);
    current_line_number++;

    // Update testing results with the result of testing the current line
    testingResults_->processRecordTestingResult(testResult);
  }

  // Get the final testing results
  const auto &finalResult = getTestingResults();

  // Return the final results
  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .data = finalResult->getResultsJson()};
}
