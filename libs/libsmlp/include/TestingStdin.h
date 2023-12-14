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
#include "Testing.h"

class TestingStdin : Testing {
public:
  using Testing::Testing;
  virtual ~TestingStdin() = default;

  void test() override {}
};