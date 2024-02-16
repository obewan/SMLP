#pragma once
#include "SimpleHTTPServerMock.h"
#include "SimpleTCPClient.h"
#include <condition_variable>
#include <mutex>

class SimpleTCPClientMock : public SimpleTCPClient {
public:
  using SimpleTCPClient::SimpleTCPClient;
  SimpleHTTPServerMock *server = nullptr;

  std::mutex wait_m;
  std::condition_variable wait_cv;

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
    server->recv_write(message.c_str());
    server->cv_data.notify_all();
  }

  std::string receive() override { return server->send_read(); }

  std::string sendAndReceive(const std::string &message) override {
    std::unique_lock lk(wait_m);
    send(message);
    wait_cv.wait_for(lk, std::chrono::seconds(5),
                     [this]() { return !server->sendQueue.empty(); });
    std::string response = receive();
    std::cout << "Client received: " << response << std::endl;
    return response;
  }

  void disconnect() override {
    if (server == nullptr) {
      return;
    }
    server->clientConnection = false;
  }
};