/**
 * @file SimpleTCPServer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief
 * @date 2023-12-06
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once

#include "Common.h"
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>

class SimpleTCPServer {
public:
  SimpleTCPServer() = default;
  SimpleTCPServer(const SimpleTCPServer &other) = delete;
  SimpleTCPServer &operator=(const SimpleTCPServer &other) = delete;
  virtual ~SimpleTCPServer() {
    if (!stopSource_.stop_requested()) {
      stop();
    }
  }

  virtual void start();
  virtual void stop();
  virtual void handle_client(int client_socket, const std::string &client_ip,
                             const std::stop_token &stoken);
  /**
   * @brief if there is an end of line or an end of chars (\0), get the line and
   * process it.
   *
   * @param line_buffer
   * @param client_info
   */
  virtual void processLineBuffer(std::string &line_buffer,
                                 const std::string &client_info);

  /**
   * @brief use the line data with the neural network, thread safe.
   *
   * @param line
   * @param client_info
   */
  virtual void processLine(const std::string &line,
                           const std::string &client_info);

  /**
   * @brief Get the TCP service started flag, thread safe.
   * @return true if server started
   * @return false if server not started
   */
  bool isStarted() const { return isStarted_.load(); }

  void setServerPort(unsigned short port) { this->sin_port_ = port; }
  unsigned short getServerPort() const { return this->sin_port_; }

  const std::string &getServerIp() const { return server_ip_; }

  void setClientBufferSize(size_t bytes_length) {
    this->client_buff_size_ = bytes_length;
  }
  size_t getClientBufferSize() const { return this->client_buff_size_; }

protected:
  std::stop_source stopSource_;
  std::vector<std::jthread> clientHandlers_;
  std::mutex threadMutex_;
  std::atomic<bool> isStarted_ = false;
  int server_socket_ = -1;
  std::string server_ip_;
  std::string server_info_;
  unsigned short sin_port_ = 8080;
  size_t client_buff_size_ = 32_K;
};
