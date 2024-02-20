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

namespace smlp {

/**
 * @brief Errors types
 *
 */
enum class Error {
  ConfigFileNotFound,
  CSVParsingError,
  CSVParsingErrorEmptyLine,
  CSVParsingErrorColumnsBadFormat,
  CSVParsingErrorColumnsMin,
  CSVParsingErrorColumnsSize,
  DifferentModelVersion,
  FailedToOpenFile,
  InternalError,
  InvalidDatasetFileMissing,
  InvalidDirectory,
  InvalidJsonFile,
  InvalidTotalInput,
  InvalidTotalOutput,
  InvalidTrainingRatioTooBig,
  InvalidTrainingRatioTooSmall,
  JsonParsingError,
  LayerTypeInvalidInput,
  LayerTypeInvalidOutput,
  LayerTypeNotRecognized,
  MissingExportFileParam,
  MissingImportFile,
  MissingNetworkToImportParam,
  TCPServerAcceptError,
  TCPServerRecvError,
  TCPSocketBindError,
  TCPSocketCreateError,
  TCPSocketListenError,
  TCPSocketNotSet,
  TestingError,
  TrainingError,
  UnimplementedActivationFunction,
  UnimplementedMode,
  UnimplementedPredictMode,
  UnknownKey,
};

/**
 * @brief Errors messages
 *
 */
const std::map<Error, std::string> errorMessages = {
    {Error::ConfigFileNotFound, "ConfigFileNotFound"},
    {Error::CSVParsingError, "CSVParsingError"},
    {Error::CSVParsingErrorEmptyLine, "CSVParsingErrorEmptyLine"},
    {Error::CSVParsingErrorColumnsBadFormat, "CSVParsingErrorColumnsBadFormat"},
    {Error::CSVParsingErrorColumnsMin, "CSVParsingErrorColumnsMin"},
    {Error::CSVParsingErrorColumnsSize, "CSVParsingErrorColumnsSize"},
    {Error::DifferentModelVersion, "DifferentModelVersion"},
    {Error::FailedToOpenFile, "FailedToOpenFile"},
    {Error::InternalError, "InternalError"},
    {Error::InvalidDatasetFileMissing, "InvalidDatasetFileMissing"},
    {Error::InvalidDirectory, "InvalidDirectory"},
    {Error::InvalidJsonFile, "InvalidJsonFile"},
    {Error::InvalidTotalInput, "InvalidTotalInput"},
    {Error::InvalidTotalOutput, "InvalidTotalOutput"},
    {Error::InvalidTrainingRatioTooBig, "InvalidTrainingRatioTooBig"},
    {Error::InvalidTrainingRatioTooSmall, "InvalidTrainingRatioTooSmall"},
    {Error::JsonParsingError, "JsonParsingError"},
    {Error::LayerTypeInvalidInput, "LayerTypeInvalidInput"},
    {Error::LayerTypeInvalidOutput, "LayerTypeInvalidOutput"},
    {Error::LayerTypeNotRecognized, "LayerTypeNotRecognized"},
    {Error::MissingExportFileParam, "MissingExportFileParam"},
    {Error::MissingImportFile, "MissingImportFile"},
    {Error::MissingNetworkToImportParam, "MissingNetworkToImportParam"},
    {Error::TCPServerAcceptError, "TCPServerAcceptError"},
    {Error::TCPServerRecvError, "TCPServerRecvError"},
    {Error::TCPSocketBindError, "TCPSocketBindError"},
    {Error::TCPSocketCreateError, "TCPSocketCreateError"},
    {Error::TCPSocketListenError, "TCPSocketListenError"},
    {Error::TCPSocketNotSet, "TCPSocketNotSet"},
    {Error::TestingError, "TestingError"},
    {Error::TrainingError, "TrainingError"},
    {Error::UnimplementedActivationFunction, "UnimplementedActivationFunction"},
    {Error::UnimplementedMode, "UnimplementedMode"},
    {Error::UnimplementedPredictMode, "UnimplementedPredictMode"},
    {Error::UnknownKey, "UnknownKey"}};

/**
 * @brief Errors default messages
 *
 */
const std::map<Error, std::string> defaultErrorMessages = {
    {Error::ConfigFileNotFound, "Config file not found: %%config_file%%"},
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
    {Error::InvalidDatasetFileMissing,
     "A dataset file is required, but file_input is missing"},
    {Error::InvalidDirectory, "Invalid directory"},
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
    {Error::MissingExportFileParam, "Missing export filename parameter"},
    {Error::MissingImportFile, "Missing file to import"},
    {Error::MissingNetworkToImportParam,
     "The network_to_import parameter is missing"},
    {Error::TCPServerAcceptError, "TCP server error during client accept."},
    {Error::TCPServerRecvError,
     "TCP server error during client reception: closing client connection."},
    {Error::TCPSocketBindError, "TCP error: failed to bind server socket."},
    {Error::TCPSocketCreateError, "TCP error: failed to create server socket."},
    {Error::TCPSocketListenError,
     "TCP error: failed to listen on server socket."},
    {Error::TCPSocketNotSet, "TCP socket not set."},
    {Error::TestingError, "Testing error."},
    {Error::TrainingError, "Training error."},
    {Error::UnimplementedActivationFunction,
     "Unimplemented Activation Function"},
    {Error::UnimplementedMode, "Unimplemented mode"},
    {Error::UnimplementedPredictMode, "Unimplemented predict mode"},
    {Error::UnknownKey, "Unknown key"}};
} // namespace smlp