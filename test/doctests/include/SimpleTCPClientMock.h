#pragma once
#include "EventMediator.h"
#include "SimpleTCPClient.h"
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace smlp {
class SimpleTCPClientMock : public SimpleTCPClient {
public:
  using SimpleTCPClient::SimpleTCPClient;
  std::shared_ptr<EventMediator> mediator = nullptr;

  void notifyServer(const Event &event) const {
    if (mediator == nullptr) {
      return;
    }
    mediator->pushServerEvent(event);
  }

  void notifyServer(const Event::Type &type,
                    const std::string &data = "") const {
    notifyServer(Event(type, data));
  }

  void connect(const std::string &host, unsigned short port) override {
    connect();
  }

  void connect() const { notifyServer(Event::Type::ClientConnection); }

  void disconnect() override {
    notifyServer(Event::Type::ClientDisconnection);
    mediator->is_client_connected = false;
  }

  void send(const std::string &message) override {
    notifyServer(Event::Type::Message, message);
  }

  std::string receive() override {
    return mediator->popClientEvent().getData();
  }

  std::string sendAndReceive(const std::string &message) override {
    send(message);
    std::string response = receive();
    return response;
  }
};
} // namespace smlp