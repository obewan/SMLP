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
#include "CommonParameters.h"
#include "Testing.h"

class TestingSocket : public Testing {
public:
  explicit TestingSocket() : Testing(TestingType::TestingSocket){};

  void test(const std::string &line = "", size_t epoch = 0,
            size_t current_line_number = 0) override {}
};