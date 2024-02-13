#include "SimpleTCPServer.h"
#include "CommonModes.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "exception/SimpleTCPException.h"
#include <algorithm>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <stop_token>
#include <string>
#include <sys/socket.h>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h> // for inet_ntop
#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET closesocket
#else
#include <arpa/inet.h> // for inet_ntop
#include <unistd.h>
#define CLOSE_SOCKET close
#endif

constexpr int MAX_REQUESTS = 50;
constexpr __time_t SERVER_ACCEPT_TIMEOUT_SECONDS = 5;
constexpr __time_t CLIENT_RECV_TIMEOUT_SECONDS = 5;

void SimpleTCPServer::start() {
  /**
   * @brief This function compares the current value of isStarted_ with the
   * value of expected. If they are equal, it assigns the second argument (true
   * in this case) to isStarted_ and returns true. If they are not equal, it
   * assigns the current value of isStarted_ to expected and returns false.
   *
   * So, in summary, this code checks if isStarted_ is false in a thread-safe
   * way. If isStarted_ is false, it sets it to true. If isStarted_ is already
   * true, it immediately returns from the current function. This can be useful
   * in scenarios where you want to ensure that a piece of code is only executed
   * once, even in a multithreaded environment.
   *
   * Warning, this will exchange the value of isStarted_
   *
   * @param expected
   */
  if (bool expected = false; !isStarted_.compare_exchange_strong(
          expected, true)) { // thread safe comparison
    return;
  }

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw SimpleSocketException("Failed to initialize winsock");
  }
#endif
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ == -1) {
    isStarted_ = false;
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketCreateError));
  }

  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(sin_port_);
  server_address.sin_addr.s_addr = INADDR_ANY;
  // Set accept timeout
  struct timeval tv;
  tv.tv_sec = SERVER_ACCEPT_TIMEOUT_SECONDS;
  tv.tv_usec = 0;
  setsockopt(server_socket_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
             sizeof tv);
  setsockopt(server_socket_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv,
             sizeof tv);

  // bind to the tcp port
  if (bind(server_socket_, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    isStarted_ = false;
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketBindError));
  }

  // If you’re working with IPv6 addresses, you can replace AF_INET with
  // AF_INET6, client_address.sin_addr with client_address.sin6_addr, and
  // INET_ADDRSTRLEN with INET6_ADDRSTRLEN
  char server_ip_buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(server_address.sin_addr), server_ip_buf,
            INET_ADDRSTRLEN);
  server_ip_ = server_ip_buf;
  server_info_ = "[" + server_ip_ + ":" + std::to_string(sin_port_) + "] ";

  SimpleLogger::LOG_INFO(server_info_, "TCP Server started.");

  // listen for clients connections
  if (listen(server_socket_, MAX_REQUESTS) == -1) {
    isStarted_ = false;
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketListenError));
  }

  while (!stopSource_.stop_requested()) {
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);
    clientInfo client_info;
    client_info.client_socket =
        accept(server_socket_, (struct sockaddr *)&client_address, &client_len);

    // If you’re working with IPv6 addresses, you can replace AF_INET with
    // AF_INET6, client_address.sin_addr with client_address.sin6_addr, and
    // INET_ADDRSTRLEN with INET6_ADDRSTRLEN
    char client_ip_buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_address.sin_addr), client_ip_buf,
              INET_ADDRSTRLEN);
    client_info.client_ip = client_ip_buf;

    if (stopSource_.stop_requested()) {
      break;
    }
    if (client_info.client_socket == -1) {
      // Check if the error is because the socket was closed
#ifdef _WIN32
      int lastError = WSAGetLastError();
      if (lastError == WSAENOTSOCK) {
#else
      if (errno == EBADF) {
#endif
        SimpleLogger::LOG_INFO(SimpleLang::Message(Message::TCPServerClosed));
        break;
      }
#ifdef _WIN32
      if (lastError != WSAEWOULDBLOCK) {
#else
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
#endif
        SimpleLogger::LOG_ERROR(clientLog(client_info),
                                SimpleLang::Error(Error::TCPServerAcceptError));
        break;
      }
      continue;
    } // end if client_socket == -1

    SimpleLogger::LOG_INFO(clientLog(client_info), " Client connection.");
    clientHandlers_.emplace_back(
        [this, client_info](std::stop_token st) {
          handle_client(client_info, st);
        },
        stopSource_.get_token());
  } // while

  // Wait for all client handlers to finish
  for (auto &handler : clientHandlers_) {
    handler.join();
  }

  SimpleLogger::LOG_INFO(server_info_, "TCP Server stop complete.");

#ifdef _WIN32
  WSACleanup();
#endif

  isStarted_ = false;
}

void SimpleTCPServer::stop() {
  SimpleLogger::LOG_INFO(server_info_, "TCP Server stop...");
  stopSource_.request_stop();
  if (server_socket_ != -1) {
    CLOSE_SOCKET(server_socket_);
  }
  server_socket_ = -1;
}

void SimpleTCPServer::handle_client(const clientInfo &client_info,
                                    const std::stop_token &stoken) {
  char buffer[client_buff_size_];
  std::string lineBuffer;
  struct timeval tv;
  tv.tv_sec = CLIENT_RECV_TIMEOUT_SECONDS;
  tv.tv_usec = 0;
  setsockopt(client_info.client_socket, SOL_SOCKET, SO_RCVTIMEO,
             (const char *)&tv,
             sizeof(tv)); // Set the timeout

  while (!stoken.stop_requested()) {
    memset(buffer, 0, client_buff_size_);

    ssize_t bytesReceived =
        recv(client_info.client_socket, buffer, client_buff_size_, 0);
    if (bytesReceived == -1) {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        SimpleLogger::LOG_ERROR(clientLog(client_info),
                                SimpleLang::Error(Error::TCPServerRecvError));
        CLOSE_SOCKET(client_info.client_socket);
        return;
      }
      // Retry receiving data
      continue;
    }

    if (bytesReceived == 0) {
      SimpleLogger::LOG_INFO(
          clientLog(client_info),
          SimpleLang::Message(Message::TCPClientDisconnected));

      // Process any remaining data in lineBuffer
      if (!lineBuffer.empty()) {
        std::string line = lineBuffer;
        lineBuffer.erase();
        processLine(line, client_info);
      }

      CLOSE_SOCKET(client_info.client_socket);
      return;
    }

    lineBuffer.append(buffer, bytesReceived);

    processLineBuffer(lineBuffer, client_info);
  }

  CLOSE_SOCKET(client_info.client_socket);
  SimpleLogger::LOG_INFO(clientLog(client_info),
                         SimpleLang::Message(Message::TCPClientDisconnected));
}

void SimpleTCPServer::processLineBuffer(std::string &line_buffer,
                                        const clientInfo &client_info) {
  size_t pos_n = line_buffer.find('\n');
  size_t pos_r = line_buffer.find('\r'); // for windows \r\n
  size_t pos_0 = line_buffer.find('\0');
  while (pos_n != std::string::npos || pos_r != std::string::npos ||
         pos_0 != std::string::npos) {
    size_t pos = std::min({pos_n, pos_r, pos_0});
    std::string line = line_buffer.substr(0, pos);
    // If it's a Windows-style newline, consume the '\n' as well
    if (pos_r != std::string::npos && pos_r == pos && pos_n == pos + 1) {
      pos++;
    }
    line_buffer.erase(0, pos + 1);

    Common::trim(line);
    if (!line.empty()) {
      processLine(line, client_info);
    }

    pos_n = line_buffer.find('\n');
    pos_r = line_buffer.find('\r');
    pos_0 = line_buffer.find('\0');
  }
}

void SimpleTCPServer::processLine(const std::string &line,
                                  const clientInfo &client_info) {
  std::scoped_lock<std::mutex> lock(threadMutex_);
  auto &manager = Manager::getInstance();
  if (manager.app_params.verbose) {
    SimpleLogger::LOG_INFO(clientLog(client_info), "[RECV FROM CLIENT] ", line);
  }
  try {
    const auto &result = manager.processTCPClient(line);
    if (manager.app_params.mode == EMode::Predictive) {
      send(client_info.client_socket, result.message->c_str(),
           result.message->length() + 1, 0);
    }

  } catch (std::exception &ex) {
    SimpleLogger::LOG_ERROR(clientLog(client_info), ex.what());
  }
}
