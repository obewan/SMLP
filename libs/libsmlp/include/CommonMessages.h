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
  TCPClientDisconnected,
  TCPServerClosed,
};

const std::map<Message, std::string> messages = {
    {Message::TCPClientDisconnected, "TCPClientDisconnected"},
    {Message::TCPServerClosed, "TCPServerClosed"}};

const std::map<Message, std::string> defaultMessages = {
    {Message::TCPClientDisconnected, "TCP client disconnected."},
    {Message::TCPServerClosed,
     "TCP Server socket was closed, stopping server."}};