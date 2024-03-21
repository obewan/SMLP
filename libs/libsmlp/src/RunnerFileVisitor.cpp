#include "RunnerFileVisitor.h"
#include "Manager.h"
#include "PredictFile.h"
#include "SimpleLogger.h"

using namespace smlp;

/**
 * @brief visit entrance, the "line" parameter is not used here, as we read a
 * file directly.
 *
 * @param line not used here
 * @return Result
 */
Result RunnerFileVisitor::visit(const std::string &line) const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;

  switch (app_params.mode) {
  case EMode::Predict:
    predict();
    break;
  case EMode::TrainOnly:
    trainOnly();
    break;
  case EMode::TestOnly:
    testOnly();
    break;
  case EMode::TrainTestMonitored:
    trainTestMonitored();
    break;
  case EMode::TrainThenTest:
    trainThenTest();
    break;
  default:
    throw ManagerException("Unimplemented mode.");
  }

  if (manager.shouldExportNetwork()) {
    manager.exportNetwork();
  }

  return {.code = smlp::make_error_code(smlp::ErrorCode::Success),
          .action = smlp::getModeStr(app_params.mode)};
}

void RunnerFileVisitor::predict() const {
  if (!predictFile_) {
    predictFile_ = std::make_unique<PredictFile>();
  }
  predictFile_->predictData();
}

void RunnerFileVisitor::trainOnly() const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  SimpleLogger::LOG_INFO("Training, using file ", app_params.data_file);
  SimpleLogger::LOG_INFO(manager.getInlineHeader());
  if (!trainingFile_) {
    trainingFile_ = std::make_unique<TrainingFile>();
  }
  trainingFile_->createDataParser();
  trainingFile_->train();
}

void RunnerFileVisitor::testOnly() const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  SimpleLogger::LOG_INFO("Testing, using file ", app_params.data_file);
  if (!testingFile_) {
    testingFile_ = std::make_unique<TestingFile>();
  }
  testingFile_->createDataParser();
  testingFile_->test();
  const auto &results =
      testingFile_->getTestingResults()->getResultsDetailled();
  SimpleLogger::LOG_INFO(results);
}

void RunnerFileVisitor::trainTestMonitored() const {
  const auto &manager = Manager::getInstance();
  const auto &app_params = manager.app_params;
  SimpleLogger::LOG_INFO("Training and testing monitored, using file ",
                         app_params.data_file);
  SimpleLogger::LOG_INFO(
      "OutputIndexToMonitor:", app_params.output_index_to_monitor, " ",
      manager.getInlineHeader());
  if (!trainingFile_) {
    trainingFile_ = std::make_unique<TrainingFile>();
  }
  if (!testingFile_) {
    testingFile_ = std::make_unique<TestingFile>();
  }
  trainingFile_->createDataParser();
  testingFile_->setDataParser(trainingFile_->getDataParser());
  trainingFile_->trainTestMonitored(testingFile_);
}

void RunnerFileVisitor::trainThenTest() const {
  trainOnly();
  testOnly();
}