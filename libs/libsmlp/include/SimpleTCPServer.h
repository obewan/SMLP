/**
 * @file SimpleTCPServer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief
 * @date 2023-12-06
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once

#include "Common.h"
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>

class SimpleTCPServer {
public:
  SimpleTCPServer() = default;
  SimpleTCPServer(const SimpleTCPServer &other) = delete;
  SimpleTCPServer &operator=(const SimpleTCPServer &other) = delete;
  ~SimpleTCPServer() {
    if (isListening) {
      stop();
    }
  }

  void start();
  void stop();
  void handle_client(int client_socket, std::stop_token stoken);

  void setServerPort(unsigned short port) { this->sin_port = port; }
  unsigned short getServerPort() const { return this->sin_port; }

  void setClientBufferSize(size_t bytes_length) {
    this->client_buff_size = bytes_length;
  }
  size_t getClientBufferSize() const { return this->client_buff_size; }

  void processLine(const std::string &line);

private:
  std::vector<std::jthread> threads;
  std::mutex threadMutex;
  std::condition_variable condition;
  std::stop_source stopSource;
  std::vector<int> client_sockets;
  int server_socket = 0;
  unsigned short sin_port = 8080;
  size_t client_buff_size = 32_K;
  std::atomic<bool> isListening = true;
};
