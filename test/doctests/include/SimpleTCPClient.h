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

namespace smlp {
class SimpleTCPClient {
public:
  SimpleTCPClient();

  virtual ~SimpleTCPClient();

  virtual void connect(const std::string &host, unsigned short port);

  virtual void send(const std::string &message);

  virtual std::string receive();

  virtual std::string sendAndReceive(const std::string &message);

  virtual int getHttpCode(const std::string &httpResponse);
  virtual std::string getHttpBody(const std::string &httpResponse);

  virtual void disconnect();

protected:
  int client_socket = -1;
};
} // namespace smlp