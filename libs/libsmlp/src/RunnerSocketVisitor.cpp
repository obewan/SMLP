#include "RunnerSocketVisitor.h"
#include "Manager.h"
#include "SimpleLogger.h"
#include "TestingSocket.h"
#include "TrainingSocket.h"
#include <memory>

using namespace smlp;

smlp::Result RunnerSocketVisitor::visit(const std::string &line) const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;

  smlp::Result result;
  switch (app_params.mode) {
  case EMode::Predict:
    result = predict(line);
    break;
  case EMode::TrainOnly:
    result = trainOnly(line);
    break;
  case EMode::TestOnly:
    result = testOnly(line);
    break;
  case EMode::TrainTestMonitored:
    result = trainTestMonitored(line);
    break;
  case EMode::TrainThenTest:
    result = trainThenTest(line);
    break;
  default:
    throw ManagerException("Unimplemented mode.");
  }

  if (manager.shouldExportNetwork()) {
    manager.exportNetwork();
  }

  result.action = smlp::getModeStr(app_params.mode);
  return result;
}

Result RunnerSocketVisitor::predict(const std::string &line) const {
  if (!predictSocket_) {
    predictSocket_ = std::make_unique<PredictSocket>();
  }
  return predictSocket_->predictData(line);
}

smlp::Result RunnerSocketVisitor::trainOnly(const std::string &line) const {
  if (!trainingSocket_) {
    trainingSocket_ = std::make_unique<TrainingSocket>();
    trainingSocket_->createDataParser();
  }
  return trainingSocket_->train(line);
}

smlp::Result RunnerSocketVisitor::testOnly(const std::string &line) const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  if (!testingSocket_) {
    testingSocket_ = std::make_unique<TestingSocket>();
    testingSocket_->createDataParser();
  }
  const auto &results = testingSocket_->test(line);
  if (app_params.verbose) {
    const auto &resultLine =
        testingSocket_->getTestingResults()->getResultsLine();
    SimpleLogger::LOG_DEBUG(resultLine);
  }
  return results;
}

smlp::Result
RunnerSocketVisitor::trainTestMonitored(const std::string &line) const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  const auto &network_params = manager.network_params;

  if (app_params.output_index_to_monitor > network_params.output_size) {
    SimpleLogger::LOG_ERROR("output_index_to_monitor > output_size: ",
                            app_params.output_index_to_monitor, ">",
                            network_params.output_size);
    return {.code = std::make_error_code(std::errc::result_out_of_range)};
  }

  if (!trainingSocket_) {
    trainingSocket_ = std::make_unique<TrainingSocket>();
    trainingSocket_->createDataParser();
  }
  if (!testingSocket_) {
    testingSocket_ = std::make_unique<TestingSocket>();
    testingSocket_->setDataParser(trainingSocket_->getDataParser());
  }
  return trainingSocket_->trainTestMonitored(testingSocket_, line);
}

smlp::Result RunnerSocketVisitor::trainThenTest(const std::string &line) const {
  trainOnly(line);
  return testOnly(line);
}