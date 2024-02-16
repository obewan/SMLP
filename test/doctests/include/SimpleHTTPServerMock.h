#pragma once
#include "SimpleHTTPServer.h"
#include "SimpleLogger.h"
#include <SimpleLang.h>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <thread>

class SimpleHTTPServerMock : public SimpleHTTPServer {
public:
  using SimpleHTTPServer::SimpleHTTPServer;

  std::atomic<bool> clientConnection = false;
  std::atomic<bool> clientIsConnected = false;
  std::atomic<bool> clientIsSendingData = false;
  std::condition_variable cv_connection;
  std::condition_variable cv_data;
  std::mutex cv_m;
  std::mutex recv_m;
  std::mutex send_m;
  std::queue<std::string> recvQueue;
  std::queue<std::string> sendQueue;

  void start() override;

  void stop() override;

  void handle_client(const clientInfo &client_info,
                     const std::stop_token &stoken) override;

  void processLine(const std::string &line,
                   const clientInfo &client_info) override;

  void recv_write(const char *src) {
    std::scoped_lock<std::mutex> lock(recv_m);
    recvQueue.emplace(src);
  }

  std::string recv_read() {
    std::scoped_lock<std::mutex> lock(recv_m);
    auto recvData = recvQueue.front();
    recvQueue.pop();
    return recvData;
  }

  void send_write(const char *src) {
    std::scoped_lock<std::mutex> lock(send_m);
    sendQueue.emplace(src);
  }

  std::string send_read() {
    std::scoped_lock<std::mutex> lock(send_m);
    auto sendData = sendQueue.front();
    sendQueue.pop();
    return sendData;
  }
};