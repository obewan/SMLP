/**
 * @file CommonMessages.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common messages
 * @date 2023-12-10
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <map>
#include <string>

namespace smlp {

enum class Message {
  NetworkModelCreate,
  NetworkModelExport,
  NetworkModelImport,
  StdinDisabled,
  TCPClientConnection,
  TCPClientDisconnection,
  TCPServerClosed,
  UsingConfigFile,
  UsingTestingFile,
  UsingTrainingFile,
  UsingTrainingMonitoredFile,
};

const std::map<Message, std::string> messages = {
    {Message::NetworkModelCreate, "NetworkModelCreate"},
    {Message::NetworkModelExport, "NetworkModelExport"},
    {Message::NetworkModelImport, "NetworkModelImport"},
    {Message::StdinDisabled, "StdinDisabled"},
    {Message::TCPClientConnection, "TCPClientConnection"},
    {Message::TCPClientDisconnection, "TCPClientDisconnection"},
    {Message::TCPServerClosed, "TCPServerClosed"},
    {Message::UsingConfigFile, "UsingConfigFile"},
    {Message::UsingTestingFile, "UsingTestingFile"},
    {Message::UsingTrainingFile, "UsingTrainingFile"},
    {Message::UsingTrainingMonitoredFile, "UsingTrainingMonitoredFile"}};

const std::map<Message, std::string> defaultMessages = {
    {Message::NetworkModelCreate, "%%network_to_import%% not found. Creating a "
                                  "new neuronal network model..."},
    {Message::NetworkModelExport,
     "Exporting neuronal network model to %%network_to_export%%..."},
    {Message::NetworkModelImport,
     "Importing neuronal network model from %%network_to_import%%..."},
    {Message::StdinDisabled, "Stdin disabled."},
    {Message::TCPClientConnection, "Client connection."},
    {Message::TCPClientDisconnection, "Client disconnection."},
    {Message::TCPServerClosed,
     "TCP Server socket was closed, stopping server."},
    {Message::UsingConfigFile, "Using config file %%config_file%%..."},
    {Message::UsingTestingFile, "Testing, using file %%data_file%%..."},
    {Message::UsingTrainingFile, "Training, using file %%data_file%%..."},
    {Message::UsingTrainingMonitoredFile,
     "Training monitored, using file %%data_file%%..."}};
} // namespace smlp