#include "SimpleTCPClient.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <iostream>

SimpleTCPClient::SimpleTCPClient() {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw std::runtime_error("Failed to initialize winsock");
  }
#endif
}

SimpleTCPClient::~SimpleTCPClient() { disconnect(); }

void SimpleTCPClient::connect(const std::string &host, unsigned short port) {
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    throw std::runtime_error("Failed to create socket");
  }

  struct sockaddr_in serv_addr {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
    throw std::runtime_error("Invalid address/ Address not supported");
  }

  if (::connect(client_socket, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0) {
    throw std::runtime_error("Connection failed");
  }
}

void SimpleTCPClient::send(const std::string &message) {
  if (::send(client_socket, message.c_str(), message.size(), 0) < 0) {
    throw std::runtime_error("Send failed");
  }
}

void SimpleTCPClient::disconnect() {
  if (client_socket != -1) {
#ifdef _WIN32
    closesocket(client_socket);
    WSACleanup();
#else
    close(client_socket);
#endif
  }
  client_socket = -1;
}