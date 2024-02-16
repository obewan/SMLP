/**
 * @file SimpleHTTPServer.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief
 * @date 2023-12-06
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once

#include "Common.h"
#include "CommonResult.h"
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

class SimpleHTTPServer {
public:
  SimpleHTTPServer() = default;
  SimpleHTTPServer(const SimpleHTTPServer &other) = delete;
  SimpleHTTPServer &operator=(const SimpleHTTPServer &other) = delete;
  virtual ~SimpleHTTPServer() {
    if (!stopSource_.stop_requested()) {
      stop();
    }
  }

  struct clientInfo {
    int client_socket;
    std::string client_ip;
    std::string str() const { return "[" + client_ip + "] "; }
  };

  struct HttpRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
  };

  /**
   * @brief parse the HTTP raw request
   *
   * @param rawRequest
   * @return HttpRequest
   */
  virtual HttpRequest parseHttpRequest(const std::string &rawRequest);

  /**
   * @brief Get the Mode From Path object
   *
   * @param path
   * @return EMode
   */
  virtual EMode getModeFromPath(const std::string &path);

  /**
   * @brief http request validation
   *
   * @param request
   * @return smlp::Result
   */
  virtual smlp::Result httpRequestValidation(const HttpRequest &request);

  /**
   * @brief start the HTTP server
   *
   */
  virtual void start();

  /**
   * @brief stop the HTTP server
   *
   */
  virtual void stop();

  /**
   * @brief handle client thread
   *
   * @param client_info
   * @param stoken
   */
  virtual void handle_client(const clientInfo &client_info,
                             const std::stop_token &stoken);
  /**
   * @brief if there is an end of line or an end of chars (\0), get the line and
   * process it.
   *
   * @param line_buffer
   * @param client_info
   */
  virtual void processLineBuffer(std::string &line_buffer,
                                 const clientInfo &client_info);

  /**
   * @brief use the line data with the neural network, thread safe.
   *
   * @param line
   * @param client_info
   */
  virtual void processLine(const std::string &line,
                           const clientInfo &client_info);

  /**
   * @brief build an HTTP response from a smlp Result
   *
   * @param result
   * @return std::string
   */
  virtual std::string buildHttpResponse(const smlp::Result &result);

  /**
   * @brief build an HTTP response from an exception.
   *
   * @param ex
   * @return std::string
   */
  virtual std::string buildHttpResponse(std::exception &ex);

  /**
   * @brief Get the TCP service started flag, thread safe.
   * @return true if server started
   * @return false if server not started
   */
  bool isStarted() const { return isStarted_.load(); }

  void setServerPort(unsigned short http_port) { http_port_ = http_port; }
  unsigned short getServerPort() const { return http_port_; }

  const std::string &getServerIp() const { return server_ip_; }

  void setClientBufferSize(size_t bytes_length) {
    this->client_buff_size_ = bytes_length;
  }
  size_t getClientBufferSize() const { return this->client_buff_size_; }

protected:
  std::stop_source stopSource_;
  std::vector<std::jthread> clientHandlers_;
  std::timed_mutex threadMutex_;
  std::condition_variable wait_cv_;
  std::mutex wait_cv_m_;
  std::atomic<bool> isStarted_ = false;
  std::atomic<bool> isTrainedButNotExported_ = false;
  int server_socket_ = -1;
  std::string server_ip_;
  std::string server_info_;
  size_t client_buff_size_ = 32_K;
  unsigned short http_port_ = 8080;
};
