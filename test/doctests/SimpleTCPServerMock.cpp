#include "SimpleTCPServerMock.h"

constexpr __time_t SERVER_ACCEPT_TIMEOUT_SECONDS = 5;
constexpr __time_t CLIENT_RECV_TIMEOUT_SECONDS = 5;

void SimpleTCPServerMock::start() {
  if (bool expected = false; !isStarted_.compare_exchange_strong(
          expected, true)) { // thread safe comparison with exchange
    return;
  }
  isStarted_ = true;

  std::unique_lock<std::mutex> lk(cv_m);
  while (!stopSource_.stop_requested()) {
    cv_connection.wait_for(lk,
                           std::chrono::seconds(SERVER_ACCEPT_TIMEOUT_SECONDS),
                           [this] { return clientConnection.load(); });
    if (clientIsConnected) {
      std::this_thread::sleep_for(
          std::chrono::seconds(1)); // timer to avoid cpu burst
      continue;
    }

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
  std::unique_lock<std::mutex> lk(cv_m);
  while (!stoken.stop_requested()) {
    memset(buffer, 0, 32_K);
    cv_data.wait_for(lk, std::chrono::seconds(CLIENT_RECV_TIMEOUT_SECONDS),
                     [this] { return clientIsSendingData.load(); });
    clientIsSendingData = false;
    size_t bytesReceived = strlen(buffer);
    if (bytesReceived == 0) {
      SimpleLogger::LOG_INFO(
          client_info, SimpleLang::Message(Message::TCPClientDisconnected));

      // Process any remaining data in lineBuffer
      if (!lineBuffer.empty()) {
        std::string line = lineBuffer;
        lineBuffer.erase();
        processLine(line, client_info);
      }

      return;
    }

    lineBuffer.append(buffer, bytesReceived);

    processLineBuffer(lineBuffer, client_info);
  }
  SimpleLogger::LOG_INFO(client_info,
                         SimpleLang::Message(Message::TCPClientDisconnected));
  clientConnection = false;
  clientIsConnected = false;
}