
#pragma once
#include <exception>
#include <string>

/**
 * @brief A custom exception class that inherits from std::exception.
 * This class is thrown when there are issues with Predict operations.
 */
class PredictException : public std::exception {
public:
  explicit PredictException(const std::string &message) : message_(message) {}
  const char *what() const noexcept override { return message_.c_str(); }

private:
  std::string message_;
};
