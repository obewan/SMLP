#include "SimpleHTTPServer.h"
#include "Common.h"
#include "CommonMessages.h"
#include "CommonModes.h"
#include "CommonResult.h"
#include "Manager.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "exception/FileParserException.h"
#include "exception/SimpleTCPException.h"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <condition_variable>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
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
  std::string httpVersion;
  requestLineStream >> request.method >> request.path >> httpVersion;

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

  std::string method = request.method;
  std::ranges::transform(method, method.begin(), ::tolower);
  if (method != "post") {
    result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
    SimpleLogger::LOG_ERROR("Unsupported HTTP method: ", request.method);
    return result;
  }

  if (request.body.empty()) {
    result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
    SimpleLogger::LOG_ERROR("Empty HTTP body");
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
    SimpleLogger::LOG_ERROR("Unsupported HTTP path: ", request.path);
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
      // wait some seconds before the next export, unless there's a notify
      wait_cv_.wait_for(lk,
                        std::chrono::seconds(EXPORT_MODEL_SCHEDULE_SECONDS));
      if (threadMutex_.try_lock_for(
              std::chrono::seconds(MUTEX_TIMEOUT_SECONDS))) {
        try {
          auto &manager = Manager::getInstance();
          if (isTrainedButNotExported_ &&
              !manager.app_params.network_to_export.empty()) {
            manager.exportNetwork();
            isTrainedButNotExported_ = false;
            SimpleLogger::LOG_INFO("Neural network file saved.");
          }
        } catch (...) {
          threadMutex_.unlock();
          throw;
        }
        threadMutex_.unlock();
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
  struct timeval tv;
  tv.tv_sec = SERVER_ACCEPT_TIMEOUT_SECONDS;
  tv.tv_usec = 0;
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

  while (!stopSource_.stop_requested()) {
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);
    clientInfo client_info;
    client_info.client_socket =
        accept(server_socket_, (struct sockaddr *)&client_address, &client_len);

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
        SimpleLogger::LOG_ERROR(client_info.str(),
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
  char buffer[client_buff_size_];
  std::string lineBuffer;
  struct timeval tv;
  tv.tv_sec = CLIENT_RECV_TIMEOUT_SECONDS;
  tv.tv_usec = 0;
  setsockopt(client_info.client_socket, SOL_SOCKET, SO_RCVTIMEO,
             (const char *)&tv,
             sizeof(tv)); // Set the timeout

  while (!stoken.stop_requested()) {
    memset(buffer, 0, client_buff_size_);

    ssize_t bytesReceived =
        recv(client_info.client_socket, buffer, client_buff_size_, 0);
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
      if (!lineBuffer.empty()) {
        std::string line = lineBuffer;
        lineBuffer.erase();
        processLine(line, client_info);
      }

      CLOSE_SOCKET(client_info.client_socket);
      return;
    }

    lineBuffer.append(buffer, bytesReceived);

    if (lineBuffer.size() > MAX_REQUEST_SIZE) {
      SimpleLogger::LOG_ERROR(
          client_info.str(), "Max request data size reach (", lineBuffer.size(),
          "): aborting and closing connection");
      CLOSE_SOCKET(client_info.client_socket);
      return;
    }

    if (Manager::getInstance().app_params.verbose) {
      SimpleLogger::LOG_DEBUG(
          client_info.str(),
          smlp::getEscapedString(std::string(buffer, bytesReceived)));
    }

    const std::string &extracted = processLineBuffer(lineBuffer);
    if (!extracted.empty()) {
      processLine(extracted, client_info);
    }
  }

  CLOSE_SOCKET(client_info.client_socket);
  SimpleLogger::LOG_INFO(client_info.str(),
                         SimpleLang::Message(Message::TCPClientDisconnection));
}

std::string SimpleHTTPServer::processLineBuffer(std::string &line_buffer) {
  if (smlp::trimALL(line_buffer).empty()) {
    return "";
  }

  std::string headers;
  std::string body;
  const std::string backup(line_buffer);
  bool inBody = false;

  size_t pos;
  while ((pos = line_buffer.find("\r\n")) != std::string::npos) {
    std::string line = line_buffer.substr(0, pos);
    line_buffer.erase(0, pos + 2); // +2 because "\r\n" is two characters

    if (!inBody) {
      if (line.empty()) {
        inBody = true;
        continue;
      } else {
        headers += line + "\r\n";
      }
    } else {
      body += line + "\r\n";
    }
  }

  if (!inBody) {
    // reset the lineBuffer for next call
    line_buffer = backup;
    return "";
  }

  // in case of http request not ended by \r\n
  if (body.empty() && !line_buffer.empty()) {
    body += line_buffer;
    line_buffer.erase();
  }

  // return the extracted request
  return (headers.ends_with("\r\n") ? headers + "\r\n" + body
                                    : headers + "\r\n\r\n" + body);
}

void SimpleHTTPServer::processLine(const std::string &line,
                                   const clientInfo &client_info) {

  if (threadMutex_.try_lock_for(std::chrono::seconds(MUTEX_TIMEOUT_SECONDS))) {
    try {
      if (smlp::trimALL(line).empty()) {
        threadMutex_.unlock();
        return;
      }
      auto &manager = Manager::getInstance();
      const auto &app_params = manager.app_params;

      // parsing
      const auto &request = parseHttpRequest(line);

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

    } catch (FileParserException &fpe) {
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

  switch (result.code.value()) {
  case 0:
  case 200:
    statusLine = "HTTP/1.1 200 OK\r\n";
    break;
  case 201:
  case 202:
  case 400:
  case 401:
  case 402:
  case 403:
  case 404:
  case 405:
  case 406:
  case 408:
  case 409:
  case 500:
  case 501:
  case 503:
    statusLine = "HTTP/1.1 " + std::to_string(result.code.value()) + " " +
                 result.code.message() + "\r\n";
    break;
  default:
    statusLine =
        "HTTP/1.1 500 Internal Server Error\r\n"; // but keep the others error
                                                  // codes for the json
    break;
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

std::string SimpleHTTPServer::buildHttpResponse(FileParserException &fpe) {
  smlp::Result result;
  result.code = smlp::make_error_code(smlp::ErrorCode::BadRequest);
  std::string statusLine = "HTTP/1.1 " + std::to_string(result.code.value()) +
                           " " + result.code.message() + "\r\n";
  std::string httpResponse = statusLine +
                             "Content-Type: text/plain\r\n"
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
                             "Content-Type: text/plain\r\n"
                             "Content-Length: " +
                             std::to_string(result.json().length()) +
                             "\r\n"
                             "\r\n" +
                             result.json();
  return httpResponse;
}
