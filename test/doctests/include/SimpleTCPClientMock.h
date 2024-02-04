#pragma once
#include "SimpleTCPClient.h"
#include "SimpleTCPServerMock.h"

class SimpleTCPClientMock : public SimpleTCPClient {
public:
  using SimpleTCPClient::SimpleTCPClient;
  SimpleTCPServerMock *server = nullptr;
  void connect(const std::string &host, unsigned short port) override {
    connect();
  }
  void connect() {
    if (server == nullptr) {
      return;
    }
    server->clientConnection = true;
    server->cv_connection.notify_all();
  }
  void send(const std::string &message) override {
    if (server == nullptr) {
      return;
    }
    std::strcpy(server->buffer, message.c_str());
    server->cv_data.notify_all();
  }
  void disconnect() override {
    if (server == nullptr) {
      return;
    }
    server->clientConnection = false;
  }
};