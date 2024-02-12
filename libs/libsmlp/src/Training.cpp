#include "Training.h"
#include "Manager.h"

Training::Training(TrainingType training_type) : trainingType(training_type) {}

Common::RecordResult Training::processInputLine(const std::string &line) const {
  Common::RecordResult result = fileParser_->processLine(line);
  if (result.isSuccess) {
    const auto &network = Manager::getInstance().network;
    network->forwardPropagation(result.record.inputs);
    network->backwardPropagation(result.record.outputs);
    network->updateWeights();
  }
  return result;
}