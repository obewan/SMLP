#pragma once
#include "TrainingException.h"

namespace smlp {
/**
 * @brief A custom exception class that inherits from TrainingException.
 * This class is thrown when there are issues during training.
 */
class TrainingFileException : public TrainingException {
public:
  using TrainingException::TrainingException;
};
} // namespace smlp