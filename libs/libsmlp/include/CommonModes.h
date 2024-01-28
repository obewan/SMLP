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

enum class EMode {
  Predictive,
  TestOnly,
  TrainOnly,
  TrainTestMonitored,
  TrainThenTest
};

enum class EInput { File, Stdin, Socket };

const std::map<std::string, EMode, std::less<>> mode_map{
    {"Predictive", EMode::Predictive},
    {"TestOnly", EMode::TestOnly},
    {"TrainOnly", EMode::TrainOnly},
    {"TrainTestMonitored", EMode::TrainTestMonitored},
    {"TrainThenTest", EMode::TrainThenTest}};

enum class EPredictiveMode { CSV, NumberAndRaw, NumberOnly, RawOnly };

const std::map<std::string, EPredictiveMode, std::less<>> predictive_map{
    {"CSV", EPredictiveMode::CSV},
    {"NumberAndRaw", EPredictiveMode::NumberAndRaw},
    {"NumberOnly", EPredictiveMode::NumberOnly},
    {"RawOnly", EPredictiveMode::RawOnly}};

enum class EActivationFunction { ELU, LReLU, PReLU, ReLU, Sigmoid, Tanh };

const std::map<std::string, EActivationFunction, std::less<>> activation_map{
    {"ELU", EActivationFunction::ELU},
    {"LReLU", EActivationFunction::LReLU},
    {"PReLU", EActivationFunction::PReLU},
    {"ReLU", EActivationFunction::ReLU},
    {"Sigmoid", EActivationFunction::Sigmoid},
    {"Tanh", EActivationFunction::Tanh}};