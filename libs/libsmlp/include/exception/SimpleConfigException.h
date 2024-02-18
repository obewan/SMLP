#pragma once
#include <exception>
#include <string>

namespace smlp {
/**
 * @brief A custom exception class that inherits from std::exception.
 * This class is thrown when there are issues during SimpleConfig.
 */
class SimpleConfigException : public std::exception {
public:
  explicit SimpleConfigException(const std::string &message)
      : message_(message) {}
  const char *what() const noexcept override { return message_.c_str(); }

private:
  std::string message_;
};
} // namespace smlp