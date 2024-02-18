/**
 * @file CommonModes.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common modes
 * @date 2023-12-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <map>
#include <string>

namespace smlp {
enum class EMode {
  Predict,
  TestOnly,
  TrainOnly,
  TrainTestMonitored,
  TrainThenTest
};

enum class EInput { File, Stdin, Socket };

const std::map<std::string, EMode, std::less<>> mode_map{
    {"Predict", EMode::Predict},
    {"TestOnly", EMode::TestOnly},
    {"TrainOnly", EMode::TrainOnly},
    {"TrainTestMonitored", EMode::TrainTestMonitored},
    {"TrainThenTest", EMode::TrainThenTest}};

enum class EPredictMode { CSV, NumberAndRaw, NumberOnly, RawOnly };

const std::map<std::string, EPredictMode, std::less<>> predict_map{
    {"CSV", EPredictMode::CSV},
    {"NumberAndRaw", EPredictMode::NumberAndRaw},
    {"NumberOnly", EPredictMode::NumberOnly},
    {"RawOnly", EPredictMode::RawOnly}};

enum class EActivationFunction { ELU, LReLU, PReLU, ReLU, Sigmoid, Tanh };

const std::map<std::string, EActivationFunction, std::less<>> activation_map{
    {"ELU", EActivationFunction::ELU},
    {"LReLU", EActivationFunction::LReLU},
    {"PReLU", EActivationFunction::PReLU},
    {"ReLU", EActivationFunction::ReLU},
    {"Sigmoid", EActivationFunction::Sigmoid},
    {"Tanh", EActivationFunction::Tanh}};
} // namespace smlp