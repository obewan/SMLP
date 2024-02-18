#pragma once
#include <string>

class Event {
public:
  enum class Type {
    ClientConnection,
    ClientConnected,
    ClientDisconnection,
    ClientDisconnected,
    Message,
    MessageReceived,
    ServerStarted,
    ServerListening,
  };

  Event(const Type &type, const std::string &data = "")
      : type_(type), data_(data) {}

  Type getType() const { return type_; }
  const std::string &getData() const { return data_; }

private:
  Type type_;
  std::string data_;
};