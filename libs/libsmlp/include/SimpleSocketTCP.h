/**
 * @file SimpleSocketTCP.h
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

class SimpleSocketTCP {
public:
  void init();
  void handle_client(int client_socket, std::stop_token stoken);
  void stop();

  void setServerPort(unsigned short port) { this->sin_port = port; }
  unsigned short getServerPort() const { return this->sin_port; }

  void setClientBufferSize(size_t bytes_length) {
    this->client_buff_size = bytes_length;
  }
  size_t getClientBufferSize() const { return this->client_buff_size; }

private:
  std::vector<std::jthread> threads;
  std::mutex threadMutex;
  std::condition_variable condition;
  std::stop_source stopSource;
  int server_socket = 0;
  unsigned short sin_port = 8080;
  size_t client_buff_size = 32_K;
};
