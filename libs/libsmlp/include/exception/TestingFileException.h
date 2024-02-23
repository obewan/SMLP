#pragma once
#include "TestingException.h"

namespace smlp {
/**
 * @brief A custom exception class that inherits from TestingException.
 * This class is thrown when there are issues during testing.
 */
class TestingFileException : public TestingException {
public:
  using TestingException::TestingException;
};
} // namespace smlp