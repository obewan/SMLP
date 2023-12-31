/**
 * @file TestingFile.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingFile
 * @date 2023-12-14
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonModes.h"
#include "SimpleLang.h"
#include "Testing.h"
#include "exception/TestingException.h"
#include <memory>

class TestingFile : public Testing {
public:
  explicit TestingFile() : Testing(TestingType::TestingFile) {}

  void test(const std::string &line = "", size_t epoch = 0,
            size_t current_line_number = 0) override;
};