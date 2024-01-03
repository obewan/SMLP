#include "SimpleTCPServer.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "exception/SimpleTCPException.h"
#include <algorithm>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
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
#define TIMEOUT_SECONDS 5

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

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketBindError));
  }

  if (listen(server_socket, MAX_REQUESTS) == -1) {
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketListenError));
  }

  // Start the watchdog thread
  std::jthread watchdog([this] {
    // Wait for a stop request
    while (!stopSource.stop_requested()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Stop request received, close the server socket
    closeServerSocket();
  });

  while (!stopSource.stop_requested()) {
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_address, &client_len);

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

    client_sockets.push_back(client_socket);

    std::stop_token stoken = stopSource.get_token();
    threads.emplace_back(&SimpleTCPServer::handle_client, this, client_socket,
                         std::ref(stoken));
  }

  // Close the server socket
  closeServerSocket();

#ifdef _WIN32
  WSACleanup();
#endif
}

void SimpleTCPServer::stop() {
  // Request all threads to stop
  stopSource.request_stop();
  for (auto &thread : threads) {
    thread.request_stop();
  }

  closeAllClientSockets();
}

void SimpleTCPServer::handle_client(int client_socket, std::stop_token stoken) {
  char buffer[client_buff_size];
  std::string lineBuffer;
  struct timeval tv;
  tv.tv_sec = TIMEOUT_SECONDS; // Timeout in seconds
  tv.tv_usec = 0;              // Not using microseconds
  setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
             sizeof(tv));

  while (!stoken.stop_requested()) {
    memset(buffer, 0, client_buff_size);

    int bytesReceived = recv(client_socket, buffer, client_buff_size, 0);
    if (bytesReceived == -1) {

      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::scoped_lock<std::mutex> lock(threadMutex);
        std::cerr << SimpleLang::Error(Error::TCPServerRecvError) << std::endl;
        closeClientSocket(client_socket);
        return;
      }
      // Retry receiving data
      continue;
    }

    if (bytesReceived == 0) {
      std::scoped_lock<std::mutex> lock(threadMutex);
      std::cout << SimpleLang::Message(Message::TCPClientDisconnected)
                << std::endl;
      closeClientSocket(client_socket);
      break;
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

  closeClientSocket(client_socket);
}

void SimpleTCPServer::processLine(const std::string &line) {
  std::scoped_lock<std::mutex> lock(threadMutex);
  Manager::getInstance().processTCPClient(line);
}

void SimpleTCPServer::closeClientSocket(int client_socket) {
  std::scoped_lock<std::mutex> lock(threadMutex);
  if (auto it = std::ranges::find(client_sockets, client_socket);
      it != client_sockets.end()) {
    CLOSE_SOCKET(client_socket);
    client_sockets.erase(it);
  }
}

void SimpleTCPServer::closeAllClientSockets() {
  std::scoped_lock<std::mutex> lock(threadMutex);
  for (auto const &socket : client_sockets) {
    CLOSE_SOCKET(socket);
  }
  client_sockets.clear();
}

void SimpleTCPServer::closeServerSocket() {
  if (server_socket != -1) {
    CLOSE_SOCKET(server_socket);
    server_socket = -1;
  }
}