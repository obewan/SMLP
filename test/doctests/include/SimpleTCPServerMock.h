#pragma once
#include "SimpleLogger.h"
#include "SimpleTCPServer.h"
#include <SimpleLang.h>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

class SimpleTCPServerMock : public SimpleTCPServer {
public:
  using SimpleTCPServer::SimpleTCPServer;
  char buffer[32_K];
  std::atomic<bool> clientConnection = false;
  std::atomic<bool> clientIsConnected = false;
  std::atomic<bool> clientIsSendingData = false;
  std::condition_variable cv_connection;
  std::condition_variable cv_data;
  std::mutex cv_m;

  void start() override;
  void stop() override;
  void handle_client(int client_socket, const std::string &client_ip,
                     const std::stop_token &stoken) override;
};