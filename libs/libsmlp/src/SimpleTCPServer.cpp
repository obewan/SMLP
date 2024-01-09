#include "SimpleTCPServer.h"
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
#include <sys/socket.h>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET closesocket
#else
#include <unistd.h>
#define CLOSE_SOCKET close
#endif

#define MAX_REQUESTS 50
#define SERVER_ACCEPT_TIMEOUT_SECONDS 5
#define CLIENT_RECV_TIMEOUT_SECONDS 5

void SimpleTCPServer::start() {

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw SimpleSocketException("Failed to initialize winsock");
  }
#endif
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketCreateError));
  }

  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(sin_port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  // Set accept timeout
  struct timeval tv;
  tv.tv_sec = SERVER_ACCEPT_TIMEOUT_SECONDS;
  tv.tv_usec = 0;
  setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
             sizeof tv);
  setsockopt(server_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv,
             sizeof tv);

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketBindError));
  }

  if (listen(server_socket, MAX_REQUESTS) == -1) {
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketListenError));
  }

  if (!isExternalServerToken) {
    serverToken = stopSource.get_token();
  }

  while (!serverToken.stop_requested()) {
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_address, &client_len);

    if (serverToken.stop_requested()) {
      break;
    }
    if (client_socket == -1) {
      // Check if the error is because the socket was closed
#ifdef _WIN32
      if (WSAGetLastError() == WSAENOTSOCK) {
#else
      if (errno == EBADF) {
#endif
        std::cout << SimpleLang::Message(Message::TCPServerClosed) << std::endl;
        break;
      }
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::cerr << SimpleLang::Error(Error::TCPServerAcceptError)
                  << std::endl;
        break;
      }
      continue;
    }

    clientHandlers.emplace_back(
        [this, client_socket](std::stop_token st) {
          handle_client(client_socket, st);
        },
        stopSource.get_token());
  } // while

  // Wait for all client handlers to finish
  for (auto &handler : clientHandlers) {
    handler.join();
  }

  SimpleLogger::getInstance().info("TCP Server stop complete.");

#ifdef _WIN32
  WSACleanup();
#endif
}

void SimpleTCPServer::start(const std::stop_token &server_token) {
  serverToken = server_token;
  isExternalServerToken = true;
  start();
}

void SimpleTCPServer::stop() {
  std::scoped_lock<std::mutex> lock(threadMutex);
  SimpleLogger::getInstance().info("TCP Server stop...");
  stopSource.request_stop();
  CLOSE_SOCKET(server_socket);
}

void SimpleTCPServer::handle_client(int client_socket,
                                    std::stop_token &stoken) {
  char buffer[client_buff_size];
  std::string lineBuffer;
  struct timeval tv;
  tv.tv_sec = CLIENT_RECV_TIMEOUT_SECONDS;
  tv.tv_usec = 0;
  setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
             sizeof(tv)); // Set the timeout

  while (!stoken.stop_requested()) {
    memset(buffer, 0, client_buff_size);

    int bytesReceived = recv(client_socket, buffer, client_buff_size, 0);
    if (bytesReceived == -1) {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::scoped_lock<std::mutex> lock(threadMutex);
        std::cerr << SimpleLang::Error(Error::TCPServerRecvError) << std::endl;
        CLOSE_SOCKET(client_socket);
        return;
      }
      // Retry receiving data
      continue;
    }

    if (bytesReceived == 0) {
      std::scoped_lock<std::mutex> lock(threadMutex);
      std::cout << SimpleLang::Message(Message::TCPClientDisconnected)
                << std::endl;
      CLOSE_SOCKET(client_socket);
      return;
    }

    lineBuffer.append(buffer, bytesReceived);

    size_t pos;
    while ((pos = lineBuffer.find('\n')) != std::string::npos) {
      std::string line = lineBuffer.substr(0, pos);
      lineBuffer.erase(0, pos + 1);
      processLine(line);
    }

    // Process any remaining data in lineBuffer
    if (!lineBuffer.empty()) {
      processLine(lineBuffer);
    }

    // Echo message back to client
    // send(client_socket, lineBuffer, bytesReceived + 1, 0);
  }

  CLOSE_SOCKET(client_socket);
}

void SimpleTCPServer::processLine(const std::string &line) {
  std::scoped_lock<std::mutex> lock(threadMutex);
  Manager::getInstance().processTCPClient(line);
}
