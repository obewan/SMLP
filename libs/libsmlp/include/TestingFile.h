/**
 * @file TestingFile.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief TestingFile
 * @date 2023-12-14
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Testing.h"

namespace smlp {
class TestingFile : public Testing {
public:
  explicit TestingFile() : Testing(TestingType::TestingFile) {}

  smlp::Result test(const std::string &line = "", size_t epoch = 0,
                    size_t current_line_number = 0) override;
};
} // namespace smlp