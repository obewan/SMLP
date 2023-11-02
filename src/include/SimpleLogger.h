/**
 * @file SimpleLogger.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief a simple logger
 * @date 2023-11-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <iostream>

class SimpleLogger {
public:
  template <typename... Args> void log(Args &&...args) const {
    std::cout << "[INFO] ";
    (std::cout << ... << args) << std::endl;
  }

  template <typename... Args> void warn(Args &&...args) const {
    std::cout << "[WARN] ";
    (std::cout << ... << args) << std::endl;
  }

  template <typename... Args> void error(Args &&...args) const {
    std::cout << "[ERROR] ";
    (std::cout << ... << args) << std::endl;
  }
};