#include "Training.h"
#include "InputOutputPair.h"
#include <fstream>
#include <iostream>
#include <string>

Training::Training(Network *network, const std::string &data_file_path,
                   Optimizer *optimizer)
    : network_(network), data_file_path_(data_file_path),
      optimizer_(optimizer){};

void Training::Train(int num_epochs) {
  std::ifstream data_file(data_file_path_);
  if (!data_file.is_open()) {
    std::cerr << "Failed to open data file: " << data_file_path_ << std::endl;
    return;
  }

  std::string line;
  for (int epoch = 0; epoch < num_epochs; epoch++) {
    // Reset the file pointer to the beginning of the file for each epoch
    data_file.seekg(0);

    while (std::getline(data_file, line)) {
      // Parse input and output values from the line
      std::vector<float> input, output;
      // TODO : parse input and output values from the line and store in input
      // and output vectors

      // Perform forward propagation and backward propagation for the current
      // example
      network_->Forward(input);

      // Store the parsed output values in the network's output layer
      network_->GetOutputLayer()->SetOutputTargetValues(output);

      // Compare the predicted output values with the sample output values and
      // compute the loss.
      network_->Backward();

      // Update weights and biases using the optimizer
      network_->UpdateWeightsAndBiases();
    }
  }

  data_file.close();
}
