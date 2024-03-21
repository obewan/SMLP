#include "RunnerStdinVisitor.h"
#include "Manager.h"
#include "PredictStdin.h"

using namespace smlp;

Result RunnerStdinVisitor::visit(const std::string &line) const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;

  smlp::Result result;
  switch (app_params.mode) {
  case EMode::Predict:
    result = predict();
    break;
  case EMode::TrainOnly:
    result = trainOnly();
    break;
  case EMode::TestOnly:
    result = testOnly();
    break;
  case EMode::TrainTestMonitored:
    result = trainTestMonitored();
    break;
  case EMode::TrainThenTest:
    result = trainThenTest();
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

Result RunnerStdinVisitor::predict() const {
  if (!predictStdin_) {
    predictStdin_ = std::make_unique<PredictStdin>();
  }
  return predictStdin_->predictData();
}

Result RunnerStdinVisitor::trainOnly() const {
  if (!trainingStdin_) {
    trainingStdin_ = std::make_unique<TrainingStdin>();
    trainingStdin_->createDataParser();
  }
  return trainingStdin_->train();
}

Result RunnerStdinVisitor::testOnly() const {
  if (!testingStdin_) {
    testingStdin_ = std::make_unique<TestingStdin>();
    testingStdin_->createDataParser();
  }
  return testingStdin_->test();
}

Result RunnerStdinVisitor::trainTestMonitored() const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  const auto &network_params = manager.network_params;

  if (app_params.output_index_to_monitor > network_params.output_size) {
    SimpleLogger::LOG_ERROR("output_index_to_monitor > output_size: ",
                            app_params.output_index_to_monitor, ">",
                            network_params.output_size);
    return {.code = std::make_error_code(std::errc::result_out_of_range)};
  }

  SimpleLogger::LOG_INFO(
      "Training and testing monitored, using command pipe input... ");
  SimpleLogger::LOG_INFO(
      "OutputIndexToMonitor:", app_params.output_index_to_monitor, " ",
      manager.getInlineHeader());

  if (!trainingStdin_) {
    trainingStdin_ = std::make_unique<TrainingStdin>();
    trainingStdin_->createDataParser();
  }
  if (!testingStdin_) {
    testingStdin_ = std::make_unique<TestingStdin>();
    testingStdin_->setDataParser(trainingStdin_->getDataParser());
  }
  return trainingStdin_->trainTestMonitored(testingStdin_);
}

Result RunnerStdinVisitor::trainThenTest() const {
  trainOnly();
  return testOnly();
}
