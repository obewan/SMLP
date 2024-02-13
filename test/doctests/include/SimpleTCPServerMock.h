#pragma once
#include "SimpleLogger.h"
#include "SimpleTCPServer.h"
#include <SimpleLang.h>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <thread>

class SimpleTCPServerMock : public SimpleTCPServer {
public:
  using SimpleTCPServer::SimpleTCPServer;

  std::atomic<bool> clientConnection = false;
  std::atomic<bool> clientIsConnected = false;
  std::atomic<bool> clientIsSendingData = false;
  std::condition_variable cv_connection;
  std::condition_variable cv_data;
  std::mutex cv_m;
  std::mutex buffer_m;
  std::queue<std::string> bufferQueue;

  void start() override;
  void stop() override;
  void handle_client(const clientInfo &client_info,
                     const std::stop_token &stoken) override;

  void buffer_write(const char *src) {
    std::scoped_lock<std::mutex> lock(buffer_m);
    bufferQueue.emplace(src);
  }

  std::string buffer_get() {
    std::scoped_lock<std::mutex> lock(buffer_m);
    auto bufferQueueElement = bufferQueue.front();
    bufferQueue.pop();
    return bufferQueueElement;
  }
};