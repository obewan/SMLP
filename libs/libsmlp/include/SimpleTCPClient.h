/**
 * @file SimpleTCPClient.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief A simple TCP client, for the TCP Server unit tests.
 * @date 2023-12-22
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once

#include <string>

class SimpleTCPClient {
public:
  SimpleTCPClient();

  ~SimpleTCPClient();

  void connect(const std::string &host, unsigned short port);

  void send(const std::string &message);

private:
  int sockfd = -1;
};