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
#include "exception/DataParserException.h"
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace smlp {
class SimpleHTTPServer {
public:
  SimpleHTTPServer() = default;
  SimpleHTTPServer(const SimpleHTTPServer &other) = delete;
  SimpleHTTPServer &operator=(const SimpleHTTPServer &other) = delete;
  virtual ~SimpleHTTPServer();

  struct clientInfo {
    std::string client_ip = "";
    std::string str() const { return "[" + client_ip + "] "; }
#ifdef _WIN32
    // Sockets are unsigned long long on Windows
    unsigned long long client_socket = 0;
#else
    // Sockets are int on Linux
    int client_socket = -1;
#endif
  };

  struct HttpRequest {
    std::string method;
    std::string path;
    std::string httpVersion;
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
   * @brief extract the request from the request buffer. This will erase the
   * buffer from extracted request.
   *
   * @param request_buffer
   * @param client_info
   */
  virtual std::string processRequestBuffer(std::stringstream &request_buffer,
                                           const clientInfo &client_info);

  /**
   * @brief process the request body data with the neural network, thread safe.
   *
   * @param request
   * @param client_info
   */
  virtual void processRequest(const std::string &request,
                              const clientInfo &client_info);

  /**
   * @brief build an HTTP response from a smlp Result
   *
   * @param result
   * @return std::string
   */
  virtual std::string buildHttpResponse(const smlp::Result &result);

  /**
   * @brief build an HTTP response from an file parser exception.
   *
   * @param fpe
   * @return std::string
   */
  virtual std::string buildHttpResponse(DataParserException &fpe);

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

protected:
  std::stop_source stopSource_;
  std::vector<std::jthread> clientHandlers_;
  std::timed_mutex threadMutex_;
  std::condition_variable wait_cv_;
  std::mutex wait_cv_m_;
  std::string server_ip_;
  std::string server_info_;
#ifdef _WIN32
  // Sockets are unsigned long long on Windows
  unsigned long long server_socket_ = 0;
#else
  // Sockets are int on Linux
  int server_socket_ = -1;
#endif
  unsigned short http_port_ = 8080;
  std::atomic<bool> isStarted_ = false;
  std::atomic<bool> isTrainedButNotExported_ = false;
};
} // namespace smlp