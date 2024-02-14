#include "doctest.h"

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include "Manager.h"
#include "SimpleTCPClientMock.h"
#include "SimpleTCPServerMock.h"
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
TEST_CASE("Testing the SimpleTCPServer class - unmocked" *
          doctest::timeout(20) * doctest::skip(true)) {
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

  // Allow previous messages to be processed
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // SendAndReceive testing
  manager.app_params.mode = EMode::Predictive;
  const auto &response = client.sendAndReceive(
      "   0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,"
      "0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00      ");

  CHECK(client.getHttpCode(response) == 200);
  CHECK(client.getHttpBody(response) ==
        "{\"code\":0,\"data\":\"1,0.04,0.57,0.8,0.08,1,0.38,0,"
        "0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0\""
        ",\"message\":\"Success\"}");

  std::string expected = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/json\r\n"
                         "Content-Length: 113\r\n\r\n"
                         "{\"code\":0,\"data\":\"1,0.04,0.57,0.8,0.08,1,0.38,0,"
                         "0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0\""
                         ",\"message\":\"Success\"}";
  CHECK(response == expected);

  MESSAGE("[TEST] Closing the TCP server and client...");

  client.disconnect();
  server.stop();
  serverThread.join();
  CHECK(server.isStarted() == false);
  MESSAGE("[TEST] End of test");
}

TEST_CASE("Testing the SimpleTCPServer class - mocked" * doctest::timeout(40)) {
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
  std::this_thread::sleep_for(std::chrono::seconds(2)); // Allow server to start
  CHECK(server->isStarted() == true);

  // Starting the client
  MESSAGE("[TEST] Starting the TCP client...");
  client.connect();
  std::this_thread::sleep_for(std::chrono::seconds(2));

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
  std::this_thread::sleep_for(std::chrono::seconds(10));

  MESSAGE("[TEST] Closing the TCP server and client...");

  client.disconnect();
  server->stop();
  serverThread.join();
  CHECK(server->isStarted() == false);

  client.server = nullptr;
  delete server;
}

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS