/**
 * @file CommonErrors.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common errors
 * @date 2023-12-02
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <map>
#include <string>

enum class Error {
  DifferentModelVersion,
  FailedToOpenFile,
  InvalidJsonFile,
  InvalidTotalInput,
  InvalidTotalOutput,
  InvalidTrainingRatio,
  JsonParsingError,
  LayerTypeInvalidInput,
  LayerTypeInvalidOutput,
  LayerTypeNotRecognized,
  MissingImportFile,
  UnimplementedActivationFunction,
  UnimplementedPredictiveMode,
  UnknownKey
};

const std::map<Error, std::string> errorMessages = {
    {Error::DifferentModelVersion, "DifferentModelVersion"},
    {Error::FailedToOpenFile, "FailedToOpenFile"},
    {Error::InvalidJsonFile, "InvalidJsonFile"},
    {Error::InvalidTotalInput, "InvalidTotalInput"},
    {Error::InvalidTotalOutput, "InvalidTotalOutput"},
    {Error::InvalidTrainingRatio, "InvalidTrainingRatio"},
    {Error::JsonParsingError, "JsonParsingError"},
    {Error::LayerTypeInvalidInput, "LayerTypeInvalidInput"},
    {Error::LayerTypeInvalidOutput, "LayerTypeInvalidOutput"},
    {Error::LayerTypeNotRecognized, "LayerTypeNotRecognized"},
    {Error::MissingImportFile, "MissingImportFile"},
    {Error::UnimplementedActivationFunction, "UnimplementedActivationFunction"},
    {Error::UnimplementedPredictiveMode, "UnimplementedPredictiveMode"},
    {Error::UnknownKey, "UnknownKey"}};

const std::map<Error, std::string> defaultErrorMessages = {
    {Error::DifferentModelVersion,
     "Your file model version (%%vuser%%) is not the same as current version "
     "(%%vcurrent%%)"},
    {Error::FailedToOpenFile, "Failed to open file"},
    {Error::InvalidJsonFile, "Invalid JSON file"},
    {Error::InvalidTotalInput, "Invalid number of input values"},
    {Error::InvalidTotalOutput,
     "The expected output size provided is not valid"},
    {Error::InvalidTrainingRatio,
     "No data is available for testing. Please check your 'training_ratio' "
     "parameter. Testing cannot proceed."},
    {Error::JsonParsingError, "JSON parsing error"},
    {Error::LayerTypeInvalidInput,
     "Invalid LayerType for first layer: not an InputLayer"},
    {Error::LayerTypeInvalidOutput,
     "Invalid LayerType for last layer: not an OutputLayer"},
    {Error::LayerTypeNotRecognized, "LayerType is not recognized"},
    {Error::MissingImportFile, "Missing file to import"},
    {Error::UnimplementedActivationFunction,
     "Unimplemented Activation Function"},
    {Error::UnimplementedPredictiveMode, "Unimplemented predictive mode"},
    {Error::UnknownKey, "Unknown key"}};