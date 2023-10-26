#pragma once
#include <exception>
#include <string>

class TestingException : public std::exception {
public:
  explicit TestingException(const std::string &message) : message_(message) {}
  const char *what() const noexcept override { return message_.c_str(); }

private:
  std::string message_;
};
