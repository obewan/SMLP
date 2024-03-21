#include "TrainingSocket.h"
#include "Common.h"
#include "Manager.h"

using namespace smlp;

smlp::Result TrainingSocket::train(const std::string &line) {
  const auto &manager = Manager::getInstance();
  const auto &processResult = processInputLine(line);
  return {.code = smlp::make_error_code(processResult.isSuccess
                                            ? smlp::ErrorCode::Success
                                            : smlp::ErrorCode::Failure),
          .action = smlp::getModeStr(manager.app_params.mode)};
}

smlp::Result
TrainingSocket::trainTestMonitored(std::unique_ptr<TestingSocket> &testing,
                                   const std::string &line) {
  const auto &processResult = processInputLine(line);
  const auto &testingResult = testing->test(line);
  return {.code = testingResult.code,
          .action = smlp::getModeStr(EMode::TrainTestMonitored),
          .data = testingResult.data};
}