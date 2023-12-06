#include "SimpleSocketTCP.h"
#include "exception/SimpleSocketException.h"
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
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

void SimpleSocketTCP::init() {
  try {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
      throw SimpleSocketException("Failed to initialize winsock");
    }
#endif
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
      throw SimpleSocketException("Failed to create socket");
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(sin_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) == -1) {
      throw SimpleSocketException("Failed to bind socket");
    }

    if (listen(server_socket, 10) == -1) {
      throw SimpleSocketException("Failed to listen on socket");
    }

    while (true) {
      sockaddr_in client_address{};
      socklen_t client_len = sizeof(client_address);
      int client_socket = accept(
          server_socket, (struct sockaddr *)&client_address, &client_len);

      if (client_socket == -1) {
        std::cerr << "Failed to accept client" << std::endl;
        continue;
      }

      std::stop_token stoken = stopSource.get_token();
      threads.emplace_back(&SimpleSocketTCP::handle_client, this, client_socket,
                           std::ref(stoken));
    }
    CLOSE_SOCKET(server_socket);

#ifdef _WIN32
    WSACleanup();
#endif
  } catch (std::exception &ex) {
    throw SimpleSocketException(ex.what());
  }
}

void SimpleSocketTCP::handle_client(int client_socket, std::stop_token stoken) {
  char buffer[client_buff_size];

  while (!stoken.stop_requested()) {
    memset(buffer, 0, client_buff_size);

    int bytesReceived = recv(client_socket, buffer, client_buff_size, 0);
    if (bytesReceived == -1) {
      std::cerr << "Error in recv(). Quitting" << std::endl;
      break;
    }

    if (bytesReceived == 0) {
      std::cout << "Client disconnected " << std::endl;
      break;
    }

    std::cout << "Received: " << std::string(buffer, 0, bytesReceived)
              << std::endl;

    // Echo message back to client
    send(client_socket, buffer, bytesReceived + 1, 0);
  }

  CLOSE_SOCKET(client_socket);
}

void SimpleSocketTCP::stop() {
  // Lock the mutex to ensure thread safety
  std::lock_guard<std::mutex> lock(threadMutex);

  // Request all threads to stop
  stopSource.request_stop();
  for (auto &thread : threads) {
    thread.request_stop();
  }

  // Close the server socket
  CLOSE_SOCKET(server_socket);

  // Join all threads
  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }

  // Notify all waiting threads
  condition.notify_all();
}