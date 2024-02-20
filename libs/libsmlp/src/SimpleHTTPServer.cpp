#include "SimpleHTTPServer.h"
#include "Common.h"
#include "CommonMessages.h"
#include "CommonModes.h"
#include "CommonResult.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "exception/DataParserException.h"
#include "exception/SimpleTCPException.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cctype>
#include <condition_variable>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <stop_token>
#include <string>
#include <sys/socket.h>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h> // for inet_ntop
#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET closesocket
#else
#include <arpa/inet.h> // for inet_ntop
#include <unistd.h>
#define CLOSE_SOCKET close
#endif

constexpr int MUTEX_TIMEOUT_SECONDS = 20;
constexpr int EXPORT_MODEL_SCHEDULE_SECONDS = 2;
constexpr int MAX_REQUESTS = 50;
constexpr size_t MAX_REQUEST_SIZE = 20_M;
constexpr __time_t SERVER_ACCEPT_TIMEOUT_SECONDS = 5;
constexpr __time_t CLIENT_RECV_TIMEOUT_SECONDS = 5;

using json = nlohmann::json;
using namespace smlp;

SimpleHTTPServer::~SimpleHTTPServer() {
  if (isStarted_ && !stopSource_.stop_requested()) {
    stop();
  }
}

SimpleHTTPServer::HttpRequest
SimpleHTTPServer::parseHttpRequest(const std::string &rawRequest) {
  HttpRequest request;
  std::istringstream requestStream(rawRequest);

  // Parse the request line
  std::string requestLine;
  std::getline(requestStream, requestLine, '\n');
  while (!requestLine.empty() && requestLine.back() == '\r') {
    requestLine.pop_back(); // Remove '\r'
  }
  std::istringstream requestLineStream(requestLine);
  requestLineStream >> request.method >> request.path >> request.httpVersion;

  // Parse the headers
  std::string headerLine;
  while (std::getline(requestStream, headerLine, '\n') && headerLine != "\r" &&
         headerLine != "\r\r") {
    while (!headerLine.empty() && headerLine.back() == '\r') {
      headerLine.pop_back(); // Remove '\r'
    }
    std::istringstream headerLineStream(headerLine);
    std::string header;
    std::getline(headerLineStream, header);

    // Split the header into name and value
    size_t colonPos = header.find(':');
    if (colonPos != std::string::npos) {
      std::string headerName = header.substr(0, colonPos);
      std::string headerValue = header.substr(colonPos + 1);

      // Trim leading and trailing whitespace
      headerName = smlp::trim(headerName);
      headerValue = smlp::trim(headerValue);

      request.headers[headerName] = headerValue;
    }
  }

  // The rest is the body
  request.body = smlp::trimALL(
      std::string(std::istreambuf_iterator<char>(requestStream), {}));

  return request;
}

EMode SimpleHTTPServer::getModeFromPath(const std::string &path) {
  std::string lower_path = path;
  std::ranges::transform(lower_path, lower_path.begin(), ::tolower);
  for (const auto &[key, value] : mode_map) {
    std::string temp_path = "/" + key;
    std::ranges::transform(temp_path, temp_path.begin(), ::tolower);
    if (temp_path == lower_path) {
      return value;
    }
  }
  // default mode
  return Manager::getInstance().app_params.mode;
}

smlp::Result SimpleHTTPServer::httpRequestValidation(
    const SimpleHTTPServer::HttpRequest &request) {
  smlp::Result result;
  result.code = smlp::make_error_code(smlp::ErrorCode::OK);
  result.action = request.path;

  std::string httpVersion = request.httpVersion;
  std::ranges::transform(httpVersion, httpVersion.begin(), ::toupper);
  if (httpVersion != "HTTP/1.0" && httpVersion != "HTTP/1.1") {
    result.code =
        smlp::make_error_code(smlp::ErrorCode::HTTPVersionNotSupported);
    SimpleLogger::LOG_ERROR(result.message());
    return result;
  }

  std::string method = request.method;
  std::ranges::transform(method, method.begin(), ::toupper);
  if (method != "POST") {
    result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
    result.technical = "Unsupported HTTP method: " + request.method;
    SimpleLogger::LOG_ERROR(result.message());
    return result;
  }

  if (request.body.empty()) {
    result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
    result.technical = "Empty HTTP body";
    SimpleLogger::LOG_ERROR(result.message());
    return result;
  }

  std::map<std::string, EMode, std::less<>> lower_mode_map;
  for (const auto &[key, value] : mode_map) {
    std::string temp_path = "/" + key;
    std::ranges::transform(temp_path, temp_path.begin(), ::tolower);
    lower_mode_map[temp_path] = value;
  }

  // Set the new mode
  std::string path = request.path;
  std::ranges::transform(path, path.begin(), ::tolower);
  if (lower_mode_map.contains(path)) {
    Manager::getInstance().app_params.mode = getModeFromPath(path);
  } else {
    result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
    result.technical = "Unsupported HTTP path: " + request.path;
    SimpleLogger::LOG_ERROR(result.message());
    return result;
  }

  return result;
}

void SimpleHTTPServer::start() {
  /**
   * @brief This function compares the current value of isStarted_ with the
   * value of expected. If they are equal, it assigns the second argument (true
   * in this case) to isStarted_ and returns true. If they are not equal, it
   * assigns the current value of isStarted_ to expected and returns false.
   *
   * So, in summary, this code checks if isStarted_ is false in a thread-safe
   * way. If isStarted_ is false, it sets it to true. If isStarted_ is already
   * true, it immediately returns from the current function. This can be useful
   * in scenarios where you want to ensure that a piece of code is only executed
   * once, even in a multithreaded environment.
   *
   * Warning, this will exchange the value of isStarted_
   *
   * @param expected
   */
  if (bool expected = false; !isStarted_.compare_exchange_strong(
          expected, true)) { // thread safe comparison
    return;
  }

  // Export periodically the network model
  auto exportModel = [this]() {
    while (!stopSource_.stop_requested()) {
      std::unique_lock lk(wait_cv_m_);
      wait_cv_.wait_for(lk,
                        std::chrono::seconds(EXPORT_MODEL_SCHEDULE_SECONDS));

      if (threadMutex_.try_lock_for(
              std::chrono::seconds(MUTEX_TIMEOUT_SECONDS))) {
        std::lock_guard lock(threadMutex_, std::adopt_lock);
        try {
          auto &manager = Manager::getInstance();
          if (isTrainedButNotExported_ &&
              !manager.app_params.network_to_export.empty()) {
            manager.exportNetwork();
            isTrainedButNotExported_ = false;
            SimpleLogger::LOG_INFO("Neural network file saved.");
          }
        } catch (const std::exception &e) {
          SimpleLogger::LOG_ERROR("Exception caught during model export: ",
                                  e.what());
        } catch (...) {
          SimpleLogger::LOG_ERROR(
              "Unknown exception caught during model export.");
        }
      }
    }
  };

#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw SimpleSocketException("Failed to initialize winsock");
  }
#endif
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ == -1) {
    isStarted_ = false;
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketCreateError));
  }

  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(getServerPort());
  server_address.sin_addr.s_addr = INADDR_ANY;

  // Set accept timeout
  struct timeval tv = {.tv_sec = SERVER_ACCEPT_TIMEOUT_SECONDS, .tv_usec = 0};
  setsockopt(server_socket_, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,
             sizeof tv);
  setsockopt(server_socket_, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv,
             sizeof tv);

  // bind to the tcp port
  if (bind(server_socket_, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    isStarted_ = false;
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketBindError));
  }

  // If you’re working with IPv6 addresses, you can replace AF_INET with
  // AF_INET6, client_address.sin_addr with client_address.sin6_addr, and
  // INET_ADDRSTRLEN with INET6_ADDRSTRLEN
  char server_ip_buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(server_address.sin_addr), server_ip_buf,
            INET_ADDRSTRLEN);
  server_ip_ = server_ip_buf;
  server_info_ =
      "[" + server_ip_ + ":" + std::to_string(getServerPort()) + "] ";

  SimpleLogger::LOG_INFO(server_info_, "TCP Server started...");

  // Create and detach the export thread
  std::jthread exportThread(exportModel);

  // listen for clients connections
  if (listen(server_socket_, MAX_REQUESTS) == -1) {
    isStarted_ = false;
    throw SimpleTCPException(SimpleLang::Error(Error::TCPSocketListenError));
  }

  try {
    while (!stopSource_.stop_requested()) {
      sockaddr_in client_address{};
      socklen_t client_len = sizeof(client_address);
      clientInfo client_info;
      client_info.client_socket = accept(
          server_socket_, (struct sockaddr *)&client_address, &client_len);

      // If you’re working with IPv6 addresses, you can replace AF_INET with
      // AF_INET6, client_address.sin_addr with client_address.sin6_addr, and
      // INET_ADDRSTRLEN with INET6_ADDRSTRLEN
      char client_ip_buf[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(client_address.sin_addr), client_ip_buf,
                INET_ADDRSTRLEN);
      client_info.client_ip = client_ip_buf;

      if (stopSource_.stop_requested()) {
        break;
      }
      if (client_info.client_socket == -1) {
        // Check if the error is because the socket was closed
#ifdef _WIN32
        int lastError = WSAGetLastError();
        if (lastError == WSAENOTSOCK) {
#else
        if (errno == EBADF) {
#endif
          SimpleLogger::LOG_INFO(SimpleLang::Message(Message::TCPServerClosed));
          break;
        }
#ifdef _WIN32
        if (lastError != WSAEWOULDBLOCK) {
#else
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
#endif
          SimpleLogger::LOG_ERROR(
              client_info.str(),
              SimpleLang::Error(Error::TCPServerAcceptError));
          break;
        }
        continue;
      } // end if client_socket == -1

      SimpleLogger::LOG_INFO(client_info.str(),
                             SimpleLang::Message(Message::TCPClientConnection));
      clientHandlers_.emplace_back(
          [this, client_info](std::stop_token st) {
            handle_client(client_info, st);
          },
          stopSource_.get_token());
    } // while

  } catch (std::exception &e) {
    SimpleLogger::LOG_ERROR(e.what());
  }

  // Wait for all client handlers to finish
  for (auto &handler : clientHandlers_) {
    handler.join();
  }

  SimpleLogger::LOG_INFO(server_info_, "Server stop complete.");

#ifdef _WIN32
  WSACleanup();
#endif

  isStarted_ = false;
}

void SimpleHTTPServer::stop() {
  SimpleLogger::LOG_INFO(server_info_, "TCP Server stop...");
  stopSource_.request_stop();
  wait_cv_.notify_all(); // notify all waiting threads
  if (server_socket_ != -1) {
    CLOSE_SOCKET(server_socket_);
  }
  server_socket_ = -1;
}

void SimpleHTTPServer::handle_client(const clientInfo &client_info,
                                     const std::stop_token &stoken) {
  char client_buff[client_buff_size_];
  std::stringstream requestBuffer;
  struct timeval tv = {.tv_sec = CLIENT_RECV_TIMEOUT_SECONDS, .tv_usec = 0};
  setsockopt(client_info.client_socket, SOL_SOCKET, SO_RCVTIMEO,
             (const char *)&tv,
             sizeof(tv)); // Set the timeout

  while (!stoken.stop_requested()) {
    try {
      memset(client_buff, 0, client_buff_size_);
      ssize_t bytesReceived =
          recv(client_info.client_socket, client_buff, client_buff_size_, 0);

      if (bytesReceived == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
          SimpleLogger::LOG_ERROR(client_info.str(),
                                  SimpleLang::Error(Error::TCPServerRecvError));
          CLOSE_SOCKET(client_info.client_socket);
          return;
        }
        // Retry receiving data
        continue;
      }

      if (bytesReceived == 0) {
        SimpleLogger::LOG_INFO(
            client_info.str(),
            SimpleLang::Message(Message::TCPClientDisconnection));
        // Process any remaining data in lineBuffer
        if (requestBuffer.rdbuf()->in_avail() != 0) {
          processRequest(requestBuffer.str(), client_info);
          requestBuffer.str(std::string());
        }
        CLOSE_SOCKET(client_info.client_socket);
        return;
      }

      requestBuffer.write(client_buff, bytesReceived);

      if (requestBuffer.str().size() > MAX_REQUEST_SIZE) {
        SimpleLogger::LOG_ERROR(
            client_info.str(), "Max request data size reach (",
            requestBuffer.str().size(), "): aborting and closing connection");
        CLOSE_SOCKET(client_info.client_socket);
        return;
      }

      if (Manager::getInstance().app_params.verbose) {
        SimpleLogger::LOG_DEBUG(
            client_info.str(),
            smlp::getEscapedString(std::string(client_buff, bytesReceived)));
      }

      const std::string &request =
          processRequestBuffer(requestBuffer, client_info);
      if (!request.empty()) {
        processRequest(request, client_info);
        requestBuffer.str(
            std::string()); // Clear requestBuffer after processing a request
        requestBuffer.clear();
      }
    } catch (std::exception &e) {
      SimpleLogger::LOG_ERROR(client_info.str(), e.what());
      const auto &httpResponse = buildHttpResponse(e);
      send(client_info.client_socket, httpResponse.c_str(),
           httpResponse.length(), 0);
      CLOSE_SOCKET(client_info.client_socket);
      return;
    }
  }

  CLOSE_SOCKET(client_info.client_socket);
  SimpleLogger::LOG_INFO(client_info.str(),
                         SimpleLang::Message(Message::TCPClientDisconnection));
}

std::string
SimpleHTTPServer::processRequestBuffer(std::stringstream &request_buffer,
                                       const clientInfo &client_info) {
  const std::string CRLF = "\r\n";
  const std::string CONTENT_LENGTH = "Content-Length: ";
  const std::string backup(request_buffer.str());
  if (smlp::trimALL(backup).empty()) {
    return "";
  }

  std::stringstream headers;
  std::stringstream body;
  bool inBody = false;
  bool hasContentLength = false;
  int contentLength = 0;

  std::string line;
  while (std::getline(request_buffer, line)) {
    // Remove '\r' from the end of the line, if present
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    if (!inBody) {
      if (line.empty()) {
        inBody = true;
        continue;
      } else {
        headers << line << CRLF;
        if (line.find(CONTENT_LENGTH) == 0) {
          hasContentLength = true;
          contentLength =
              std::stoi(smlp::trimALL(line.substr(CONTENT_LENGTH.size())));
        }
      }
    } else {
      body << line << CRLF;
    }
  }
  // Create a new stringstream with the remaining content and swap it with the
  // old one to remove request_buffer of extracted lines
  std::stringstream remaining_stream;
  remaining_stream << request_buffer.rdbuf();
  request_buffer.swap(remaining_stream);

  // in case of http request not ended by \r\n
  if (inBody && !request_buffer.str().empty()) {
    body << request_buffer.str();
    request_buffer.str(std::string());
    request_buffer.clear();
  }

  // in case of incomplete request, reset the request buffer for next call
  if (!inBody || (hasContentLength &&
                  static_cast<int>(body.str().length()) < contentLength)) {
    request_buffer.str(backup);
    request_buffer.clear();
    return "";
  }

  // return the extracted request
  return headers.str() + CRLF + body.str();
}

void SimpleHTTPServer::processRequest(const std::string &rawRequest,
                                      const clientInfo &client_info) {

  if (threadMutex_.try_lock_for(std::chrono::seconds(MUTEX_TIMEOUT_SECONDS))) {
    try {
      if (smlp::trimALL(rawRequest).empty()) {
        threadMutex_.unlock();
        return;
      }
      auto &manager = Manager::getInstance();
      const auto &app_params = manager.app_params;

      // parsing
      const auto &request = parseHttpRequest(rawRequest);

      // validation
      const auto &validation = httpRequestValidation(request);
      if (!validation.isSuccess()) {
        SimpleLogger::LOG_ERROR(client_info.str(), validation.message());
        const auto &httpResponseInvalid = buildHttpResponse(validation);
        send(client_info.client_socket, httpResponseInvalid.c_str(),
             httpResponseInvalid.length(), 0);
        threadMutex_.unlock();
        return;
      }

      // processing
      const auto &result = manager.processTCPClient(request.body);
      if (app_params.mode == EMode::TrainOnly ||
          app_params.mode == EMode::TrainThenTest ||
          app_params.mode == EMode::TrainTestMonitored) {
        isTrainedButNotExported_ = true;
      }

      // send response
      const auto &httpResponse = buildHttpResponse(result);
      send(client_info.client_socket, httpResponse.c_str(),
           httpResponse.length(), 0);

    } catch (DataParserException &fpe) {
      SimpleLogger::LOG_ERROR(client_info.str(), fpe.what());
      const auto &httpResponse = buildHttpResponse(fpe);
      send(client_info.client_socket, httpResponse.c_str(),
           httpResponse.length(), 0);
      threadMutex_.unlock();

    } catch (std::exception &ex) {
      SimpleLogger::LOG_ERROR(client_info.str(), ex.what());
      const auto &httpResponse = buildHttpResponse(ex);
      send(client_info.client_socket, httpResponse.c_str(),
           httpResponse.length(), 0);
      threadMutex_.unlock();
    }

    threadMutex_.unlock();
  }
}

std::string SimpleHTTPServer::buildHttpResponse(const smlp::Result &result) {
  std::string statusLine;

  // map values to http status
  if (result.code.value() == static_cast<int>(ErrorCode::Success)) {
    statusLine = "HTTP/1.1 200 OK\r\n";
  } else if (result.code.value() < 200) { // Failure and std::errc errors
    statusLine = "HTTP/1.1 500 Internal Server Error\r\n";
  } else { // HTTP codes
    statusLine = "HTTP/1.1 " + std::to_string(result.code.value()) + " " +
                 result.code.message() + "\r\n";
  }

  std::string httpResponse = statusLine +
                             "Content-Type: application/json\r\n"
                             "Content-Length: " +
                             std::to_string(result.json().length()) +
                             "\r\n"
                             "\r\n" +
                             result.json();
  return httpResponse;
}

std::string SimpleHTTPServer::buildHttpResponse(DataParserException &fpe) {
  smlp::Result result;
  result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
  std::string statusLine = "HTTP/1.1 " + std::to_string(result.code.value()) +
                           " " + result.code.message() + "\r\n";
  std::string httpResponse = statusLine +
                             "Content-Type: application/json\r\n"
                             "Content-Length: " +
                             std::to_string(result.json().length()) +
                             "\r\n"
                             "\r\n" +
                             result.json();
  return httpResponse;
}

std::string SimpleHTTPServer::buildHttpResponse(std::exception &ex) {
  // for safety, not going to send the server exception message to the client.
  smlp::Result result;
  result.code = smlp::make_error_code(smlp::ErrorCode::InternalServerError);
  std::string statusLine = "HTTP/1.1 " + std::to_string(result.code.value()) +
                           " " + result.code.message() + "\r\n";
  std::string httpResponse = statusLine +
                             "Content-Type: application/json\r\n"
                             "Content-Length: " +
                             std::to_string(result.json().length()) +
                             "\r\n"
                             "\r\n" +
                             result.json();
  return httpResponse;
}
