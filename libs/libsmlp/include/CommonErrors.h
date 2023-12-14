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
  CSVParsingError,
  CSVParsingErrorEmptyLine,
  CSVParsingErrorColumnsBadFormat,
  CSVParsingErrorColumnsMin,
  CSVParsingErrorColumnsSize,
  DifferentModelVersion,
  FailedToOpenFile,
  InternalError,
  InvalidJsonFile,
  InvalidTotalInput,
  InvalidTotalOutput,
  InvalidTrainingRatioTooBig,
  InvalidTrainingRatioTooSmall,
  JsonParsingError,
  LayerTypeInvalidInput,
  LayerTypeInvalidOutput,
  LayerTypeNotRecognized,
  MissingImportFile,
  UnimplementedActivationFunction,
  UnimplementedPredictiveMode,
  UnknownKey,
};

const std::map<Error, std::string> errorMessages = {
    {Error::CSVParsingError, "CSVParsingError"},
    {Error::CSVParsingErrorEmptyLine, "CSVParsingErrorEmptyLine"},
    {Error::CSVParsingErrorColumnsBadFormat, "CSVParsingErrorColumnsBadFormat"},
    {Error::CSVParsingErrorColumnsMin, "CSVParsingErrorColumnsMin"},
    {Error::CSVParsingErrorColumnsSize, "CSVParsingErrorColumnsSize"},
    {Error::DifferentModelVersion, "DifferentModelVersion"},
    {Error::FailedToOpenFile, "FailedToOpenFile"},
    {Error::InternalError, "InternalError"},
    {Error::InvalidJsonFile, "InvalidJsonFile"},
    {Error::InvalidTotalInput, "InvalidTotalInput"},
    {Error::InvalidTotalOutput, "InvalidTotalOutput"},
    {Error::InvalidTrainingRatioTooBig, "InvalidTrainingRatioTooBig"},
    {Error::InvalidTrainingRatioTooSmall, "InvalidTrainingRatioTooSmall"},
    {Error::JsonParsingError, "JsonParsingError"},
    {Error::LayerTypeInvalidInput, "LayerTypeInvalidInput"},
    {Error::LayerTypeInvalidOutput, "LayerTypeInvalidOutput"},
    {Error::LayerTypeNotRecognized, "LayerTypeNotRecognized"},
    {Error::MissingImportFile, "MissingImportFile"},
    {Error::UnimplementedActivationFunction, "UnimplementedActivationFunction"},
    {Error::UnimplementedPredictiveMode, "UnimplementedPredictiveMode"},
    {Error::UnknownKey, "UnknownKey"}};

const std::map<Error, std::string> defaultErrorMessages = {
    {Error::CSVParsingError, "CSV parsing error at line %%line_number%%"},
    {Error::CSVParsingErrorEmptyLine,
     "CSV parsing error at line %%line_number%%: empty line."},
    {Error::CSVParsingErrorColumnsBadFormat,
     "CSV parsing error at line %%line_number%%: bad format."},
    {Error::CSVParsingErrorColumnsMin,
     "CSV parsing error at line %%line_number%%: there are %%value%% columns "
     "instead of a minimum of %%expected%%."},
    {Error::CSVParsingErrorColumnsSize,
     "CSV parsing error at line %%line_number%%: there are %%value%% columns "
     "instead of %%expected%%."},
    {Error::DifferentModelVersion,
     "Your file model version (%%vuser%%) is not the same as current version "
     "(%%vcurrent%%)"},
    {Error::FailedToOpenFile, "Failed to open file"},
    {Error::InternalError, "Internal error"},
    {Error::InvalidJsonFile, "Invalid JSON file"},
    {Error::InvalidTotalInput, "Invalid number of input values"},
    {Error::InvalidTotalOutput,
     "The expected output size provided is not valid"},
    {Error::InvalidTrainingRatioTooBig,
     "No data is available for testing. Please check your 'training_ratio' "
     "parameter. Testing cannot proceed."},
    {Error::InvalidTrainingRatioTooSmall,
     "Invalid parameter: training_ratio is too small, no data for training"},
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