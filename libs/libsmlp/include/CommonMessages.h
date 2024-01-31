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

enum class Message {
  StdinDisabled,
  TCPClientDisconnected,
  TCPServerClosed,
  UsingConfigFile,
};

const std::map<Message, std::string> messages = {
    {Message::StdinDisabled, "StdinDisabled"},
    {Message::TCPClientDisconnected, "TCPClientDisconnected"},
    {Message::TCPServerClosed, "TCPServerClosed"},
    {Message::UsingConfigFile, "UsingConfigFile"}};

const std::map<Message, std::string> defaultMessages = {
    {Message::StdinDisabled, "Stdin disabled."},
    {Message::TCPClientDisconnected, "TCP client disconnected."},
    {Message::TCPServerClosed,
     "TCP Server socket was closed, stopping server."},
    {Message::UsingConfigFile, "Using config file %%config_file%%..."}};