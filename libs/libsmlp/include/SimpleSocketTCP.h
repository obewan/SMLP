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

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class SimpleSocketTCP {
public:
  void init();
  void handle_client(int client_socket, std::stop_token stoken);
  void stop();

  unsigned short sin_port = 8080;

private:
  std::vector<std::jthread> threads;
  std::mutex threadMutex;
  std::condition_variable condition;
  std::stop_source stopSource;
  int server_socket = 0;
};
