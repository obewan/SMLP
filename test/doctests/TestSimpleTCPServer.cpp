#include "CommonModes.h"
#include "doctest.h"
#include <stdexcept>

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include "Manager.h"
#include "SimpleTCPClient.h"
#include "SimpleTCPServer.h"
#include <iostream>
#include <sstream>
#include <thread>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

DOCTEST_MSVC_SUPPRESS_WARNING(4626)

/** Beware:
 * networking testing must be disable for Github so add skip(true) after the
 * Test Case name to disable, or use mocks. Also the TCP classes are and should
 * be ignored by the code coverage, as they must be mocked.
 * For doctest decorators see
 * https://github.com/doctest/doctest/blob/master/doc/markdown/testcases.md#decorators
 * For doctest concurrency examples see
 * https://github.com/doctest/doctest/blob/master/examples/all_features/concurrency.cpp
 */
TEST_CASE("Testing the SimpleTCPServer class" * doctest::timeout(10) *
          doctest::skip(true)) {
  SimpleTCPServer server;
  CHECK(server.isStarted() == false);

  // Building the network
  auto &manager = Manager::getInstance();
  manager.app_params.input = EInput::Socket;
  manager.app_params.verbose = true;
  manager.app_params.mode = EMode::TrainThenTest;
  manager.app_params.network_to_import = "../data/testModel.json";
  manager.resetTraining();
  manager.resetTesting();
  CHECK_NOTHROW(manager.importOrBuildNetwork());
  CHECK(manager.network != nullptr);

  // Starting the server
  MESSAGE("[TEST] Starting the TCP server...");
  std::jthread serverThread([&server]() { server.start(); });
  std::this_thread::sleep_for(std::chrono::seconds(2)); // Allow server to start
  CHECK(server.isStarted() == true);

  // Starting the client
  SimpleTCPClient client;
  MESSAGE("[TEST] Starting the TCP client...");
  // you can also use a custom server IP addr, instead of
  // server.getServerIp(): to get the IP with bash:
  // ip addr show eth0 | grep -oP '(?<=inet\s)\d+(\.\d+){3}'
  client.connect(server.getServerIp(), server.getServerPort());

  // Message testing
  MESSAGE("[TEST] Sending first data...");
  client.send("0,0.08,0.43,0.90,0.42,1.00,0.62,0.33,0.38,0.10,0.07,0.00,0.00,"
              "0.38,0.00,0.00,1.00,0.92,0.00,1.00,0.00");
  MESSAGE("[TEST] Sending next data...");
  client.send("1,0.01,0.57,0.90,0.25,1.00,0.00,0.67,0.92,0.09,0.02,0.00,"
              "0.00,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00");
  MESSAGE("[TEST] Sending next data...");
  client.send("oops");
  MESSAGE("[TEST] Sending next data...");
  client.send("           "); // trim check
  MESSAGE("[TEST] Sending next data...");
  client.send("   1.00,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,"
              "0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00      ");

  // Allow message to be processed
  std::this_thread::sleep_for(std::chrono::seconds(2));

  MESSAGE("[TEST] Closing the TCP server and client...");

  client.disconnect();
  server.stop();
  serverThread.join();
  CHECK(server.isStarted() == false);
  MESSAGE("[TEST] End of test");
}

class SimpleTCPServerMock : public SimpleTCPServer {
public:
  SimpleTCPServerMock() : SimpleTCPServer(){};
  char buffer[32_K];
  std::atomic<bool> clientConnection = false;
  std::atomic<bool> clientIsConnected = false;

  void start() override {
    if (bool expected = false; !isStarted_.compare_exchange_strong(
            expected, true)) { // thread safe comparison with exchange
      return;
    }
    isStarted_ = true;

    while (!stopSource_.stop_requested()) {
      if (!clientConnection || clientIsConnected) {
        // thread safe comparison, here clientIsConnected should be true
        std::this_thread::sleep_for(
            std::chrono::seconds(1)); // timer to avoid cpu burst
        continue;
      }

      int client_socket = 1;
      std::string client_ip = "localhost";
      std::string client_info = "[" + client_ip + "] ";

      SimpleLogger::LOG_INFO(client_info, " Client connection.");

      clientHandlers_.emplace_back(
          [this, client_socket, client_ip](std::stop_token st) {
            handle_client(client_socket, client_ip, st);
          },
          stopSource_.get_token());

      clientIsConnected = true;
    }
  }
  void stop() override {
    stopSource_.request_stop();
    isStarted_ = false;
  }
  void handle_client(int client_socket, const std::string &client_ip,
                     const std::stop_token &stoken) override {
    std::string client_info = "[" + client_ip + "] ";
    std::string lineBuffer;
    while (!stoken.stop_requested()) {
      memset(buffer, 0, 32_K);
      std::this_thread::sleep_for(
          std::chrono::seconds(2)); // time for client to write the buffer
      size_t bytesReceived = strlen(buffer);
      if (bytesReceived == 0) {
        SimpleLogger::LOG_INFO(
            client_info, SimpleLang::Message(Message::TCPClientDisconnected));

        // Process any remaining data in lineBuffer
        if (!lineBuffer.empty()) {
          std::string line = lineBuffer;
          lineBuffer.erase();
          processLine(line, client_info);
        }

        return;
      }

      lineBuffer.append(buffer, bytesReceived);

      processLineBuffer(lineBuffer, client_info);
    }
    SimpleLogger::LOG_INFO(client_info,
                           SimpleLang::Message(Message::TCPClientDisconnected));
    clientConnection = false;
    clientIsConnected = false;
  }
};

class SimpleTCPClientMock : public SimpleTCPClient {
public:
  SimpleTCPClientMock() : SimpleTCPClient(){};
  SimpleTCPServerMock *server = nullptr;
  void connect(const std::string &host, unsigned short port) override {}
  void connect() {
    if (server == nullptr) {
      return;
    }
    server->clientConnection = true;
  }
  void send(const std::string &message) override {
    if (server == nullptr) {
      return;
    }
    std::strcpy(server->buffer, message.c_str());
  }
  void disconnect() override {
    if (server == nullptr) {
      return;
    }
    server->clientConnection = false;
  }
};

TEST_CASE("Testing the SimpleTCPServer class - mocked" * doctest::timeout(10)) {
  auto server = new SimpleTCPServerMock();
  CHECK(server->isStarted() == false);
  SimpleTCPClientMock client;
  client.server = server;

  // Building the network
  auto &manager = Manager::getInstance();
  manager.app_params.input = EInput::Socket;
  manager.app_params.verbose = true;
  manager.app_params.mode = EMode::TrainThenTest;
  manager.app_params.network_to_import = "../data/testModel.json";
  manager.resetTraining();
  manager.resetTesting();
  CHECK_NOTHROW(manager.importOrBuildNetwork());
  CHECK(manager.network != nullptr);

  // Starting the server
  MESSAGE("[TEST] Starting the TCP server...");
  std::jthread serverThread([server]() { server->start(); });
  std::this_thread::sleep_for(std::chrono::seconds(1)); // Allow server to start
  CHECK(server->isStarted() == true);

  // Starting the client
  MESSAGE("[TEST] Starting the TCP client...");
  client.connect();

  // Message testing
  MESSAGE("[TEST] Sending first data...");
  client.send("0,0.08,0.43,0.90,0.42,1.00,0.62,0.33,0.38,0.10,0.07,0.00,0.00,"
              "0.38,0.00,0.00,1.00,0.92,0.00,1.00,0.00");
  MESSAGE("[TEST] Sending next data...");
  client.send("1,0.01,0.57,0.90,0.25,1.00,0.00,0.67,0.92,0.09,0.02,0.00,"
              "0.00,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00");
  MESSAGE("[TEST] Sending next data...");
  client.send("oops");
  MESSAGE("[TEST] Sending next data...");
  client.send("           "); // trim check
  MESSAGE("[TEST] Sending next data...");
  client.send("   1.00,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,"
              "0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00      ");

  // Allow message to be processed
  std::this_thread::sleep_for(std::chrono::seconds(2));

  MESSAGE("[TEST] Closing the TCP server and client...");

  client.disconnect();
  server->stop();
  serverThread.join();
  CHECK(server->isStarted() == false);

  client.server = nullptr;
  delete server;
}

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS