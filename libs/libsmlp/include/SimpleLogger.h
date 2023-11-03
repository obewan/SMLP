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
  template <typename... Args> void out(Args &&...args) const {
    (std::cout << ... << args) << std::endl;
  }

  template <typename... Args> void append(Args &&...args) const {
    (std::cout << ... << args);
  }

  template <typename... Args> void log(Args &&...args) const {
    std::cout << "[INFO] ";
    (std::cout << ... << args) << std::endl;
  }

  template <typename... Args> void log_append(Args &&...args) const {
    std::cout << "[INFO] ";
    (std::cout << ... << args);
  }

  template <typename... Args> void warn(Args &&...args) const {
    std::cerr << "[WARN] ";
    (std::cerr << ... << args) << std::endl;
  }

  template <typename... Args> void error(Args &&...args) const {
    std::cerr << "[ERROR] ";
    (std::cerr << ... << args) << std::endl;
  }
};