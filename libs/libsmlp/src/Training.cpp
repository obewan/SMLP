#include "Training.h"
#include "Manager.h"

Training::Training(TrainingType training_type) : trainingType(training_type) {}

smlp::RecordResult Training::processInputLine(const std::string &line) const {
  smlp::RecordResult result = fileParser_->processLine(line, false);
  if (result.isSuccess) {
    const auto &network = Manager::getInstance().network;
    network->forwardPropagation(result.record.inputs);
    network->backwardPropagation(result.record.outputs);
    network->updateWeights();
  }
  return result;
}