/**
 * @file SimpleLogger.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief a simple logger, using fluent interfaces.
 * @date 2023-11-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

enum class LogLevel { INFO, WARN, ERROR, DEBUG };

class SimpleLogger {
public:
  template <typename... Args>
  const SimpleLogger &log(LogLevel level, bool endl = true,
                          Args &&...args) const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&now_c), "%F %T") << "] ";

    switch (level) {
    case LogLevel::INFO:
      std::cout << "[INFO] ";
      break;
    case LogLevel::WARN:
      std::cout << "[WARN] ";
      break;
    case LogLevel::ERROR:
      std::cout << "[ERROR] ";
      break;
    case LogLevel::DEBUG:
      std::cout << "[DEBUG] ";
      break;
    default:
      break;
    }

    std::cout.precision(current_precision);
    (std::cout << ... << args);
    if (endl) {
      std::cout << std::endl;
    }

    return *this;
  }

  template <typename... Args> const SimpleLogger &append(Args &&...args) const {
    std::cout.precision(current_precision);
    (std::cout << ... << args);
    return *this;
  }

  template <typename... Args> const SimpleLogger &out(Args &&...args) const {
    std::cout.precision(current_precision);
    (std::cout << ... << args);
    std::cout << std::endl;
    return *this;
  }

  template <typename... Args> const SimpleLogger &info(Args &&...args) const {
    return log(LogLevel::INFO, true, args...);
  }

  template <typename... Args> const SimpleLogger &warn(Args &&...args) const {
    return log(LogLevel::WARN, true, args...);
  }

  template <typename... Args> const SimpleLogger &error(Args &&...args) const {
    return log(LogLevel::ERROR, true, args...);
  }

  template <typename... Args> const SimpleLogger &debug(Args &&...args) const {
    return log(LogLevel::DEBUG, true, args...);
  }

  const SimpleLogger &endl() const {
    std::cout << std::endl;
    std::cout.flush();
    return *this;
  }

  const SimpleLogger &setPrecision(std::streamsize precision) const {
    current_precision = precision;
    return *this;
  }

  const SimpleLogger &resetPrecision() const {
    current_precision = default_precision;
    return *this;
  }

private:
  std::streamsize default_precision = std::cout.precision();
  mutable std::streamsize current_precision = std::cout.precision();
};