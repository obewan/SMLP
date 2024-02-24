#pragma once
#include "ImportExportException.h"

namespace smlp {
/**
 * @brief EmptyCellException
 *
 */
class EmptyCellException : public ImportExportException {
public:
  using ImportExportException::ImportExportException;
};

} // namespace smlp