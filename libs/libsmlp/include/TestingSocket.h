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
#include "SimpleLogger.h"
#include "Testing.h"
#include "exception/TestingException.h"

class TestingSocket : public Testing {
public:
  explicit TestingSocket() : Testing(TestingType::TestingSocket){};

  void test(const std::string &line = "", size_t epoch = 0,
            size_t current_line_number = 0) override {
    Common::RecordResult result = fileParser_->processLine(line);
    if (!result.isSuccess) {
      throw TestingException(SimpleLang::Error(Error::TestingError));
    }
    auto testResult = testLine(result, fileParser_->current_line_number);
    current_line_number++;
    testingResults_->processRecordTestingResult(testResult);
  }
};