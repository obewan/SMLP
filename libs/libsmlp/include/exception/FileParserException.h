
#pragma once
#include <exception>
#include <string>

class FileParserException : public std::exception {
public:
  explicit FileParserException(const std::string &message)
      : message_(message) {}
  const char *what() const noexcept override { return message_.c_str(); }

private:
  std::string message_;
};
