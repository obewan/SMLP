#include "SimpleTCPServerMock.h"

constexpr __time_t SERVER_ACCEPT_TIMEOUT_SECONDS = 5;
constexpr __time_t CLIENT_RECV_TIMEOUT_SECONDS = 5;

void SimpleTCPServerMock::start() {
  if (bool expected = false; !isStarted_.compare_exchange_strong(
          expected, true)) { // thread safe comparison with exchange
    return;
  }
  isStarted_ = true;
  clientIsConnected = false;
  std::unique_lock<std::mutex> lk(cv_m);

  while (!stopSource_.stop_requested()) {
    if (clientIsConnected) {
      cv_connection.wait_for(lk, std::chrono::seconds(1)); // to avoid cpu burst
      continue;
    }

    cv_connection.wait_for(lk,
                           std::chrono::seconds(SERVER_ACCEPT_TIMEOUT_SECONDS),
                           [this] { return clientConnection.load(); });

    int client_socket = 1;
    std::string client_ip = "localhost";
    std::string client_info = "[" + client_ip + "] ";

    SimpleLogger::LOG_INFO(client_info, " Client connection.");

    clientHandlers_.emplace_back(
        [this, client_socket, client_ip](std::stop_token st) {
          handle_client(client_socket, client_ip, st);
        },
        stopSource_.get_token());

    clientIsConnected = true;
  }
}

void SimpleTCPServerMock::stop() {
  stopSource_.request_stop();
  isStarted_ = false;
}

void SimpleTCPServerMock::handle_client(int client_socket,
                                        const std::string &client_ip,
                                        const std::stop_token &stoken) {
  std::string client_info = "[" + client_ip + "] ";
  std::string lineBuffer;
  std::unique_lock lk(cv_m);

  while (!stoken.stop_requested()) {
    SimpleLogger::LOG_INFO(client_info, "MOCK TEST - SERVER WAITING FOR DATA");
    cv_data.wait_for(lk, std::chrono::seconds(CLIENT_RECV_TIMEOUT_SECONDS),
                     [this] { return clientIsSendingData.load(); });

    clientIsSendingData = false;

    while (!bufferQueue.empty()) {
      auto bufferQueueElement = buffer_get();
      size_t bytesReceived = bufferQueueElement.size();

      lineBuffer.append(bufferQueueElement.c_str(), bytesReceived + 1);
      SimpleLogger::LOG_INFO(
          client_info, "MOCK TEST - SERVER DATA PROCESSING: ", lineBuffer);
      processLineBuffer(lineBuffer, client_info);
    }
  }
  SimpleLogger::LOG_INFO(client_info,
                         SimpleLang::Message(Message::TCPClientDisconnected));
  clientConnection = false;
  clientIsConnected = false;
}