/**
 * @file TestingStdin.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingStdin
 * @date 2023-12-14
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonParameters.h"
#include "Testing.h"
#include <cstddef>
#include <iostream>

class TestingStdin : public Testing {
public:
  explicit TestingStdin() : Testing(TestingType::TestingStdin) {}

  void test(const std::string &line = "", size_t epoch = 0,
            size_t current_line_number = 0) override;
};