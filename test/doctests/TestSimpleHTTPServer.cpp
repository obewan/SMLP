#include "CommonModes.h"
#include "SimpleHTTPServer.h"
#include "doctest.h"

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include "Manager.h"
#include "SimpleHTTPServerMock.h"
#include "SimpleTCPClientMock.h"
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
          doctest::timeout(40) * doctest::skip(true)) {
  SimpleHTTPServer server;
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

  // SendAndReceive testing
  const auto &httpRequest =
      "POST /predict HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 11\r\n"
      "\r\n"
      "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response = client.sendAndReceive(httpRequest);

  CHECK(manager.app_params.mode == EMode::Predict);

  CHECK(client.getHttpCode(response) == 200);
  CHECK(client.getHttpBody(response) ==
        "{\"action\":\"Predict\",\"code\":0,\"data\":\"1,0.04,0.57,0.8,0.08,1,"
        "0.38,0,"
        "0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0\""
        ",\"message\":\"Success\"}");

  std::string expected = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/json\r\n"
                         "Content-Length: 132\r\n\r\n"
                         "{\"action\":\"Predict\",\"code\":0,\"data\":\"1,0.04,"
                         "0.57,0.8,0.08,1,0.38,0,"
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
  auto server = new SimpleHTTPServerMock();
  CHECK(server->isStarted() == false);
  SimpleTCPClientMock client;
  client.server = server;
  std::mutex wait_m;
  std::condition_variable wait_cv;
  std::unique_lock lk(wait_m);

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
  wait_cv.wait_for(lk, std::chrono::seconds(2),
                   [server]() { return server->isStarted(); });
  CHECK(server->isStarted() == true);

  // Starting the client
  MESSAGE("[TEST] Starting the TCP client...");
  client.connect();
  wait_cv.wait_for(lk, std::chrono::seconds(2),
                   [server]() { return server->clientIsConnected.load(); });

  // SendAndReceive testing
  SUBCASE("Testing POST predict") {
    const auto &httpRequest =
        "POST /predict HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
        "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
    const auto &response = client.sendAndReceive(httpRequest);

    CHECK(manager.app_params.mode == EMode::Predict);

    CHECK(client.getHttpCode(response) == 200);
    CHECK(
        client.getHttpBody(response) ==
        R"({"action":"Predict","code":0,"data":"1,0.04,0.57,0.8,0.08,1,0.38,0,0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0","message":"Success"})");

    std::string expected =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 132\r\n\r\n"
        R"({"action":"Predict","code":0,"data":"1,0.04,0.57,0.8,0.08,1,0.38,0,0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0","message":"Success"})";
    CHECK(response == expected);
  }

  SUBCASE("Testing POST trainOnly") {
    const auto &httpRequest =
        "POST /trainonly HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "0.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
        "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
    const auto &response = client.sendAndReceive(httpRequest);

    CHECK(manager.app_params.mode == EMode::TrainOnly);

    CHECK(client.getHttpCode(response) == 200);
    CHECK(client.getHttpBody(response) ==
          R"({"action":"TrainOnly","code":0,"data":"","message":"Success"})");
  }

  MESSAGE("[TEST] Closing the TCP server and client...");

  client.disconnect();
  server->stop();
  serverThread.join();
  CHECK(server->isStarted() == false);

  client.server = nullptr;
  delete server;
}

TEST_CASE("Testing the SimpleTCPServer class - inner methods") {
  SUBCASE("Testing parseHttpRequest") {
    const std::string &rawRequest =
        "POST /predict HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
        "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
    SimpleHTTPServer server;
    const auto &httpRequest = server.parseHttpRequest(rawRequest);
    CHECK(httpRequest.headers.size() == 3);
    CHECK(httpRequest.headers.at("Host") == "localhost");
    CHECK(httpRequest.headers.at("Content-Type") == "text/plain");
    CHECK(httpRequest.headers.at("Content-Length") == "11");
    CHECK(httpRequest.method == "POST");
    CHECK(httpRequest.path == "/predict");
    CHECK(httpRequest.body ==
          "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0."
          "00,0.00,1.00,0.92,0.00,1.00,0.00");
  }

  SUBCASE("Testing getModeFromPath") {
    SimpleHTTPServer server;
    CHECK(server.getModeFromPath("/predict") == EMode::Predict);
    CHECK(server.getModeFromPath("/testonly") == EMode::TestOnly);
    CHECK(server.getModeFromPath("/trainonly") == EMode::TrainOnly);
    CHECK(server.getModeFromPath("/trainthentest") == EMode::TrainThenTest);
    CHECK(server.getModeFromPath("/traintestmonitored") ==
          EMode::TrainTestMonitored);
  }

  SUBCASE("Testing httpRequestValidation") {
    const std::string &rawRequest =
        "POST /predict HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
        "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
    SimpleHTTPServer server;
    const auto &httpRequest = server.parseHttpRequest(rawRequest);
    auto &manager = Manager::getInstance();
    manager.app_params.mode = EMode::TestOnly;
    const auto &httpValidation = server.httpRequestValidation(httpRequest);
    CHECK(httpValidation.code.value() == 200);
    CHECK(manager.app_params.mode == EMode::Predict);
  }
}

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS