#include "SimpleTCPServer.h"
#include "Manager.h"
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

void SimpleTCPServer::start() {

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw SimpleSocketException("Failed to initialize winsock");
  }
#endif

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    throw SimpleTCPException("Failed to create socket");
  }

  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(sin_port);
  server_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    throw SimpleTCPException("Failed to bind socket");
  }

  if (listen(server_socket, 10) == -1) {
    throw SimpleTCPException("Failed to listen on socket");
  }

  isListening = true;

  while (isListening) {
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&client_address, &client_len);

    if (client_socket == -1) {
      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::cerr << "Error in accept(). Quitting" << std::endl;
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
  CLOSE_SOCKET(server_socket);

#ifdef _WIN32
  WSACleanup();
#endif
}

void SimpleTCPServer::stop() {
  // Gracefully stop accepting new clients
  isListening = false;

  // Request all threads to stop
  stopSource.request_stop();
  for (auto &thread : threads) {
    thread.request_stop();
  }

  // Notify all waiting threads to terminate
  condition.notify_all();

  // Join all remaining threads
  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  // Close all client sockets
  for (auto const &socket : client_sockets) {
    CLOSE_SOCKET(socket);
  }
  client_sockets.clear();
}

void SimpleTCPServer::handle_client(int client_socket, std::stop_token stoken) {
  char buffer[client_buff_size];
  std::string lineBuffer;

  while (!stoken.stop_requested()) {
    memset(buffer, 0, client_buff_size);

    int bytesReceived = recv(client_socket, buffer, client_buff_size, 0);
    if (bytesReceived == -1) {

      if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::scoped_lock<std::mutex> lock(threadMutex);
        std::cerr << "Error in recv(). Closing client connection" << std::endl;
        CLOSE_SOCKET(client_socket);
        client_sockets.erase(std::ranges::find(client_sockets, client_socket));
        return;
      }
      // Retry receiving data
      continue;
    }

    if (bytesReceived == 0) {
      std::scoped_lock<std::mutex> lock(threadMutex);
      std::cout << "Client disconnected " << std::endl;
      CLOSE_SOCKET(client_socket);
      client_sockets.erase(std::ranges::find(client_sockets, client_socket));
      break;
    }

    lineBuffer.append(buffer, bytesReceived);

    size_t pos;
    while ((pos = lineBuffer.find('\n')) != std::string::npos) {
      std::string line = lineBuffer.substr(0, pos);
      lineBuffer.erase(0, pos + 1);
      processLine(line);
    }

    // Echo message back to client
    // send(client_socket, buffer, bytesReceived + 1, 0);
  }

  // Process any remaining data in lineBuffer
  if (!lineBuffer.empty()) {
    processLine(lineBuffer);
  }

  // Close the client socket
  CLOSE_SOCKET(client_socket);
}

void SimpleTCPServer::processLine(const std::string &line) {
  std::scoped_lock<std::mutex> lock(threadMutex);
  std::cout << "Received line: " << line << std::endl;
  Manager::getInstance().processTCPClient(line);
}
