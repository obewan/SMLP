#include "SimpleTCPClient.h"
#include "SimpleTCPServer.h"
#include "doctest.h"
#include <iostream>
#include <sstream>
#include <thread>

/** Beware:
 * networking testing must be disable for Github so add skip(true) after the
 * Test Case name to disable, or use mocks. Also the TCP classes are and should
 * be ignored by the code coverage, as they must be mocked. For doctest
 * decorators see
 * https://github.com/doctest/doctest/blob/master/doc/markdown/testcases.md#decorators
 */
TEST_CASE("Testing the SimpleTCPServer class" * doctest::timeout(10) *
          doctest::skip(true)) {
  // change this by your WSL IP addr, if using WSL:
  // ip addr show eth0 | grep -oP '(?<=inet\s)\d+(\.\d+){3}'
  std::string server_ip_addr = "172.22.67.125";
  SimpleTCPServer server;
  std::cout << "[TEST] Starting the TCP server..." << std::endl;
  std::jthread serverThread([&server] { server.start(); });

  // Allow server to start
  std::this_thread::sleep_for(std::chrono::seconds(1));

  SimpleTCPClient client;
  std::cout << "[TEST] Starting the TCP client..." << std::endl;
  client.connect(server_ip_addr, server.getServerPort());
  client.send("Test message");

  // Allow message to be processed
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Redirect cout to a stringstream
  std::stringstream ss;
  auto oldCoutBuffer = std::cout.rdbuf();
  std::cout.rdbuf(ss.rdbuf());

  // Check server's output
  CHECK(ss.str() == "Received line: Test message\n");

  // Restore cout's original buffer
  std::cout.rdbuf(oldCoutBuffer);

  std::cout << "[TEST] Closing the TCP server and client..." << std::endl;
  server.stop();
  serverThread.join();
}

TEST_CASE("Testing the SimpleTCPServer class - mock") {
  // TODO
}