#include "SimpleTCPClient.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <cstring>
#include <iostream>

constexpr int CLIENT_RECV_BUFFER_SIZE = 4096;
constexpr long CLIENT_RECV_TIMEOUT_SECONDS = 10;

using namespace smlp;

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
  struct timeval timeout;
  timeout.tv_sec = CLIENT_RECV_TIMEOUT_SECONDS; // Timeout after 10 seconds
  timeout.tv_usec = 0;

  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    throw std::runtime_error("Client failed to create socket");
  }

  // Set the timeout for the socket
  if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout,
                 sizeof(timeout)) < 0) {
    throw std::runtime_error("Client failed to set timeout");
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
  if (::send(client_socket, message.c_str(), message.size() + 1, 0) <
      0) { // we add +1 to add the null character (\0)
    throw std::runtime_error("Client send failed");
  }
}

std::string SimpleTCPClient::receive() {
  char buffer[CLIENT_RECV_BUFFER_SIZE]; // Create a buffer to hold the incoming
                                        // message
  memset(buffer, 0, sizeof(buffer));    // Initialize the buffer

  // Receive the message from the server
  int bytesReceived = ::recv(client_socket, buffer, sizeof(buffer) - 1, 0);
  if (bytesReceived < 0) {
    throw std::runtime_error("Client receive failed");
  }

  // Return the received message as a string
  return std::string(buffer, bytesReceived);
}

std::string SimpleTCPClient::sendAndReceive(const std::string &message) {
  // Send the message to the server
  if (::send(client_socket, message.c_str(), message.size() + 1, 0) < 0) {
    throw std::runtime_error("Client send failed");
  }

  // Receive the response from the server
  std::string response = receive();

  // Print the response
  std::cout << "Client received: " << response << std::endl;

  return response;
}

int SimpleTCPClient::getHttpCode(const std::string &httpResponse) {
  // Extract the status line
  std::size_t endOfStatusLine = httpResponse.find("\r\n");
  std::string statusLine = httpResponse.substr(0, endOfStatusLine);

  // Extract the status code
  std::size_t statusCodeStart = statusLine.find(' ') + 1;
  std::size_t statusCodeEnd = statusLine.find(' ', statusCodeStart);
  std::string statusCodeStr =
      statusLine.substr(statusCodeStart, statusCodeEnd - statusCodeStart);
  int statusCode = std::stoi(statusCodeStr);

  return statusCode;
}

std::string SimpleTCPClient::getHttpBody(const std::string &httpResponse) {
  // Find the end of the headers
  std::size_t endOfHeaders = httpResponse.find("\r\n\r\n");

  // Extract the body
  std::string body = httpResponse.substr(endOfHeaders + 4);

  // Check if the body ends with \r\n and remove it
  if (body.size() >= 2 && body.substr(body.size() - 2) == "\r\n") {
    body = body.substr(0, body.size() - 2);
  }

  return body;
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