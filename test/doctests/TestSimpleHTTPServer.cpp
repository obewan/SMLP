#include "Common.h"
#include "doctest.h"

#ifndef DOCTEST_CONFIG_NO_EXCEPTIONS

DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_BEGIN
#include "CommonModes.h"
#include "EventMediator.h"
#include "Manager.h"
#include "SimpleHTTPServer.h"
#include "SimpleHTTPServerMock.h"
#include "SimpleTCPClientMock.h"
#include <memory>
DOCTEST_MAKE_STD_HEADERS_CLEAN_FROM_WARNINGS_ON_WALL_END

DOCTEST_MSVC_SUPPRESS_WARNING(4626)

using namespace smlp;

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

TEST_CASE("Testing the SimpleTCPServer class - mocked" * doctest::timeout(20)) {
  SimpleHTTPServerMock server;
  SimpleTCPClientMock client;
  auto mediator = std::make_shared<EventMediator>();
  server.mediator = mediator;
  client.mediator = mediator;
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
  auto ev1 = mediator->popClientEvent();
  auto ev2 = mediator->popClientEvent();
  CHECK(ev1.getType() == Event::Type::ServerStarted);
  CHECK(ev2.getType() == Event::Type::ServerListening);
  CHECK(server.isStarted() == true);

  // Starting the client
  MESSAGE("[TEST] Starting the TCP client...");
  client.connect();
  auto ev3 = mediator->popClientEvent();
  CHECK(ev3.getType() == Event::Type::ClientConnected);

  MESSAGE("[TEST] Testing POST Predict");
  const auto &httpRequest1 =
      "POST /predict HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 55\r\n"
      "\r\n"
      "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response1 = client.sendAndReceive(httpRequest1);
  CHECK(manager.app_params.mode == EMode::Predict);
  CHECK(client.getHttpCode(response1) == 200);
  CHECK(
      client.getHttpBody(response1) ==
      R"({"action":"Predict","code":0,"data":"1,0.04,0.57,0.8,0.08,1,0.38,0,0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0","message":"Success"})");
  std::string expected1 =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: application/json\r\n"
      "Content-Length: 132\r\n\r\n"
      R"({"action":"Predict","code":0,"data":"1,0.04,0.57,0.8,0.08,1,0.38,0,0.85,0.12,0.05,0,0.73,0.62,0,0,1,0.92,0,1,0","message":"Success"})";
  CHECK(response1 == expected1);

  MESSAGE("[TEST] Testing POST TrainOnly");
  const auto &httpRequest2 =
      "POST /trainonly HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 59\r\n"
      "\r\n"
      "0.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response2 = client.sendAndReceive(httpRequest2);
  CHECK(manager.app_params.mode == EMode::TrainOnly);
  CHECK(client.getHttpCode(response2) == 200);
  CHECK(client.getHttpBody(response2) ==
        R"({"action":"TrainOnly","code":0,"data":"","message":"Success"})");

  MESSAGE("[TEST] Testing POST TestOnly");
  const auto &httpRequest3 =
      "POST /testonly HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 59\r\n"
      "\r\n"
      "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response3 = client.sendAndReceive(httpRequest3);
  CHECK(manager.app_params.mode == EMode::TestOnly);
  CHECK(client.getHttpCode(response3) == 200);
  CHECK(
      client.getHttpBody(response3) ==
      R"({"action":"TestOnly","code":0,"data":"{\"accuracy_avg\":100.0,\"accuracy_high\":100.0,\"accuracy_low\":100.0,\"convergence\":0.0,\"convergence_one\":0.0,\"convergence_zero\":0.0}","message":"Success"})");

  MESSAGE("[TEST] Testing POST TrainThenTest");
  const auto &httpRequest4 =
      "POST /trainthentest HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 59\r\n"
      "\r\n"
      "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response4 = client.sendAndReceive(httpRequest4);
  CHECK(manager.app_params.mode == EMode::TrainThenTest);
  CHECK(client.getHttpCode(response4) == 200);
  CHECK(
      client.getHttpBody(response4) ==
      R"({"action":"TrainThenTest","code":0,"data":"{\"accuracy_avg\":100.0,\"accuracy_high\":100.0,\"accuracy_low\":100.0,\"convergence\":0.0,\"convergence_one\":0.0,\"convergence_zero\":0.0}","message":"Success"})");

  MESSAGE("[TEST] Testing POST TrainTestMonitored");
  const auto &httpRequest5 =
      "POST /traintestmonitored HTTP/1.1\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 59\r\n"
      "\r\n"
      "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response5 = client.sendAndReceive(httpRequest5);
  CHECK(manager.app_params.mode == EMode::TrainTestMonitored);
  CHECK(client.getHttpCode(response5) == 200);
  CHECK(
      client.getHttpBody(response5) ==
      R"({"action":"TrainTestMonitored","code":0,"data":"{\"accuracy_avg\":100.0,\"accuracy_high\":100.0,\"accuracy_low\":100.0,\"convergence\":null,\"convergence_one\":null,\"convergence_zero\":null}","message":"Success"})");

  MESSAGE("[TEST] Testing unsupported HTTP version");
  const auto &httpRequest6 =
      "POST /traintestmonitored HTTP/2.0\r\n"
      "Host: localhost\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 59\r\n"
      "\r\n"
      "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
      "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
  const auto &response6 = client.sendAndReceive(httpRequest6);
  CHECK(manager.app_params.mode == EMode::TrainTestMonitored);
  CHECK(client.getHttpCode(response6) == 505);
  CHECK(
      client.getHttpBody(response6) ==
      R"({"action":"/traintestmonitored","code":505,"data":"","message":"HTTP Version Not Supported"})");

  MESSAGE("[TEST] Closing the TCP server and client...");

  client.disconnect();
  server.stop();
  serverThread.join();
  CHECK(server.isStarted() == false);
}

TEST_CASE("Testing the SimpleTCPServer class - inner methods") {
  SUBCASE("Testing processRequestBuffer") {
    const std::string &rawRequest =
        "POST /testonly HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.74.0\r\n"
        "Accept: */*\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 103\r\n"
        "\r\n"
        "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00";
    std::stringstream buffer;
    buffer << rawRequest;
    auto server = new SimpleHTTPServer();
    SimpleHTTPServer::clientInfo ci;
    const std::string &extracted = server->processRequestBuffer(buffer, ci);
    CHECK_MESSAGE(extracted == rawRequest + "\r\n",
                  smlp::getEscapedString(extracted));
    buffer.seekg(0); // move current pos to 0
    CHECK(
        buffer.peek() ==
        std::char_traits<char>::eof()); // no character after pos 0 so is empty
    delete server;
  }

  SUBCASE("Testing processRequestBuffer - splitted") {
    const std::string &rawRequest =
        "POST /testonly HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.74.0\r\n"
        "Accept: */*\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 103\r\n"
        "\r\n"
        "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00";
    auto server = new SimpleHTTPServer();
    SimpleHTTPServer::clientInfo ci;

    std::stringstream buffer1;
    const std::string split1 = rawRequest.substr(
        0, sizeof("POST /testonly HTTP/1.1\r\nHost: localhost:8080\r\n"));
    buffer1 << split1;
    const std::string &extracted1 = server->processRequestBuffer(buffer1, ci);
    CHECK_MESSAGE(extracted1.empty(), smlp::getEscapedString(extracted1));
    buffer1.seekp(0, std::ios::end);           // get current pos
    CHECK(buffer1.tellp() == split1.length()); // check current pos

    std::stringstream buffer2;
    const std::string split2 = rawRequest.substr(
        0,
        sizeof("POST /testonly HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: "
               "curl/7.74.0\r\nAccept: */*\r\nContent-Type: text/plain\r\n"
               "Content-Length: 103\r\n\r\n1.0,0.04,0.57"));
    buffer2 << split2;
    const std::string &extracted2 = server->processRequestBuffer(buffer2, ci);
    CHECK_MESSAGE(extracted2.empty(), smlp::getEscapedString(extracted2));
    buffer2.seekp(0, std::ios::end);           // get current pos
    CHECK(buffer2.tellp() == split2.length()); // check current pos

    std::stringstream buffer3;
    buffer3 << rawRequest;
    const std::string &extracted3 = server->processRequestBuffer(buffer3, ci);
    CHECK_MESSAGE(extracted3 == rawRequest + "\r\n",
                  smlp::getEscapedString(extracted3));
    buffer3.seekg(0); // move current pos to 0
    CHECK(
        buffer3.peek() ==
        std::char_traits<char>::eof()); // no character after pos 0 so is empty

    delete server;
  }

  SUBCASE("Testing parseHttpRequest") {
    const std::string &rawRequest =
        "POST /predict HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 59\r\n"
        "\r\n"
        "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,"
        "0.00,0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
    SimpleHTTPServer server;
    const auto &httpRequest = server.parseHttpRequest(rawRequest);
    CHECK(httpRequest.headers.size() == 3);
    CHECK(httpRequest.headers.at("Host") == "localhost");
    CHECK(httpRequest.headers.at("Content-Type") == "text/plain");
    CHECK(httpRequest.headers.at("Content-Length") == "59");
    CHECK(httpRequest.method == "POST");
    CHECK(httpRequest.path == "/predict");
    CHECK(httpRequest.body ==
          "0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,0."
          "00,0.00,1.00,0.92,0.00,1.00,0.00");
  }

  SUBCASE("Testing parseHttpRequest (curl)") {
    const std::string &rawRequest =
        "POST /testonly HTTP/1.1\r\r\n"
        "Host: localhost:8080\r\r\n"
        "User-Agent: curl/7.74.0\r\r\n"
        "Accept: */*\r\r\n"
        "Content-Type: text/plain\r\r\n"
        "Content-Length: 103\r\r\n"
        "\r\r\n"
        "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00\r\n";
    SimpleHTTPServer server;
    const auto &httpRequest = server.parseHttpRequest(rawRequest);
    CHECK(httpRequest.headers.size() == 5);
    CHECK(httpRequest.headers.at("Host") == "localhost:8080");
    CHECK(httpRequest.headers.at("Content-Type") == "text/plain");
    CHECK(httpRequest.headers.at("Content-Length") == "103");
    CHECK(httpRequest.method == "POST");
    CHECK(httpRequest.path == "/testonly");
    CHECK(httpRequest.body.length() > 0);
    CHECK(
        httpRequest.body ==
        "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00");
  }

  SUBCASE("Testing parseHttpRequest (curl) 2") {
    const std::string &rawRequest =
        "POST /testonly HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.74.0\r\n"
        "Accept: */*\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 103\r\n"
        "\r\n"
        "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00";
    SimpleHTTPServer server;
    const auto &httpRequest = server.parseHttpRequest(rawRequest);
    CHECK(httpRequest.headers.size() == 5);
    CHECK(httpRequest.headers.at("Host") == "localhost:8080");
    CHECK(httpRequest.headers.at("Content-Type") == "text/plain");
    CHECK(httpRequest.headers.at("Content-Length") == "103");
    CHECK(httpRequest.method == "POST");
    CHECK(httpRequest.path == "/testonly");
    CHECK(httpRequest.body.length() > 0);
    CHECK(
        httpRequest.body ==
        "1.0,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,0.73,0.62,"
        "0.00,0.00,1.00,0.92,0.00,1.00,0.00");
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
        "Content-Length: 55\r\n"
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

  SUBCASE("Testing others") {
    SimpleHTTPServer server;
    server.setServerPort(8089);
    CHECK(server.getServerPort() == 8089);
    CHECK(server.getServerIp() == std::string());
  }
}

#endif // DOCTEST_CONFIG_NO_EXCEPTIONS