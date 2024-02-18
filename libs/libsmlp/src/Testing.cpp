#include "Testing.h"
#include "Manager.h"

using namespace smlp;

TestingResult::TestResults
Testing::testLine(const smlp::RecordResult &record_result,
                  const size_t line_number, const size_t epoch) const {

  auto predicteds = Manager::getInstance().network->forwardPropagation(
      record_result.record.inputs);
  return {.epoch = epoch,
          .lineNumber = line_number,
          .expecteds = record_result.record.outputs,
          .outputs = predicteds};
}