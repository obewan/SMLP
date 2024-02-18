#pragma once
#include <exception>
#include <string>

namespace smlp {
/**
 * @brief A custom exception class that inherits from std::exception.
 * This class is thrown when there are generic exceptions during SMLP.
 */
class SmlpException : public std::exception {
public:
  explicit SmlpException(const std::string &message) : message_(message) {}
  const char *what() const noexcept override { return message_.c_str(); }

private:
  std::string message_;
};
} // namespace smlp