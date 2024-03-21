/**
 * @file TestingSocket.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingSocket
 * @date 2023-12-14
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonErrors.h"
#include "CommonParameters.h"
#include "CommonResult.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "Testing.h"
#include "exception/TestingException.h"

namespace smlp {
class TestingSocket : public Testing {
public:
  explicit TestingSocket() : Testing(TestingType::TestingSocket){};

  smlp::Result test(const std::string &line = "", size_t epoch = 0,
                    size_t current_line_number = 0) override {
    smlp::RecordResult result = dataParser_->processLine(line, true);
    if (!result.isSuccess) {
      throw TestingException(SimpleLang::Error(Error::TestingError));
    }
    auto testResult = testLine(result, dataParser_->current_line_number);
    current_line_number++;
    testingResults_->processRecordTestingResult(testResult);

    const auto &finalResult = getTestingResults();
    return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
            .data = finalResult->getResultsJson()};
  }
};
} // namespace smlp