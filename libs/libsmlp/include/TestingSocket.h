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
#include "Testing.h"

class TestingSocket : Testing {
public:
  using Testing::Testing;
  virtual ~TestingSocket() = default;

  void test() override {}
};