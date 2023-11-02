/**
 * @file Common.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common tools and objects
 * @date 2023-10-22
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <map>
#include <string>
#include <vector>

/**
 * @brief Record of inputs vector and outputs vector, in that order.
 *
 */
using Record = std::pair<std::vector<float>, std::vector<float>>;

enum class EMode {
  Predictive,
  TestOnly,
  TrainOnly,
  TrainTestMonitored,
  TrainThenTest
};

const std::map<std::string, EMode, std::less<>> mode_map{
    {"Predictive", EMode::Predictive},
    {"TestOnly", EMode::TestOnly},
    {"TrainOnly", EMode::TrainOnly},
    {"TrainTestMonitored", EMode::TrainTestMonitored},
    {"TrainThenTest", EMode::TrainThenTest}};

enum class EPredictiveMode { CSV, NumberOnly, NumberAndProba, ProbaOnly };

const std::map<std::string, EPredictiveMode, std::less<>> predictive_map{
    {"CSV", EPredictiveMode::CSV},
    {"NumberOnly", EPredictiveMode::NumberOnly},
    {"NumberAndProba", EPredictiveMode::NumberAndProba},
    {"ProbaOnly", EPredictiveMode::ProbaOnly}};

enum class EActivationFunction { ELU, LReLU, PReLU, ReLU, Sigmoid, Tanh };

const std::map<std::string, EActivationFunction, std::less<>> activation_map{
    {"ELU", EActivationFunction::ELU},
    {"LReLU", EActivationFunction::LReLU},
    {"PReLU", EActivationFunction::PReLU},
    {"ReLU", EActivationFunction::ReLU},
    {"Sigmoid", EActivationFunction::Sigmoid},
    {"Tanh", EActivationFunction::Tanh}};

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
  size_t num_epochs = 3;
  size_t output_index_to_monitor = 1;
  float training_ratio = 0.7f;
  bool output_at_end = false;
  bool verbose = false;
  EMode mode = EMode::TrainThenTest;
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

/**
 * @brief Results of a parsed record.
 */
struct RecordResult {
  bool isSuccess = false;
  bool isLineSkipped = false;
  bool isEndOfFile = false;
  bool isEndOfTrainingLines = false;
  Record record;
};

/**
 * @brief Common class.
 */
class Common {
public:
  static std::string getModeStr(EMode value) {
    for (const auto &[key, mode] : mode_map) {
      if (mode == value) {
        return key;
      }
    }
    return "Undefined";
  }

  static std::string getActivationStr(EActivationFunction value) {
    for (const auto &[key, mode] : activation_map) {
      if (mode == value) {
        return key;
      }
    }
    return "Undefined";
  }
};