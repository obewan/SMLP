/**
 * @file CommonParameters.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common parameters
 * @date 2023-12-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "CommonModes.h"
#include <map>
#include <string>

/**
 * @brief Application parameters
 *
 */
struct AppParameters {
  std::string title = "SMLP - Simple Multilayer Perceptron";
  std::string version = "1.0.0";
  std::string network_to_import = "";
  std::string network_to_export = "";
  std::string data_file = "";
  std::string config_file = "smlp.conf";
  size_t num_epochs = 1;
  size_t output_index_to_monitor = 0;
  float training_ratio = 0.7f;
  size_t training_ratio_line = 0;
  bool output_at_end = false;
  bool verbose = false;
  bool use_stdin = false;
  bool use_socket = false;
  bool disable_stdin = false;
  bool use_training_ratio_line = false;
  EMode mode = EMode::TrainThenTest;
  EPredictiveMode predictive_mode = EPredictiveMode::CSV;
};

/**
 * @brief Parameters for the neural Network.
 */
struct NetworkParameters {
  size_t input_size = 0;
  size_t hidden_size = 10;
  size_t output_size = 1;
  size_t hiddens_count = 1;
  float learning_rate = 0.01f;
  float hidden_activation_alpha = 0.1f; // used for ELU and PReLU
  float output_activation_alpha = 0.1f; // used for ELU and PReLU
  EActivationFunction hidden_activation_function = EActivationFunction::Sigmoid;
  EActivationFunction output_activation_function = EActivationFunction::Sigmoid;
};