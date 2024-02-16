#include "SimpleHTTPServerMock.h"
#include "Manager.h"

constexpr __time_t SERVER_ACCEPT_TIMEOUT_SECONDS = 5;
constexpr __time_t CLIENT_RECV_TIMEOUT_SECONDS = 5;
constexpr int MUTEX_TIMEOUT_SECONDS = 20;

void SimpleHTTPServerMock::start() {
  if (bool expected = false; !isStarted_.compare_exchange_strong(
          expected, true)) { // thread safe comparison with exchange
    return;
  }
  isStarted_ = true;
  clientIsConnected = false;
  std::unique_lock lk(cv_m);

  while (!stopSource_.stop_requested()) {
    if (clientIsConnected) {
      cv_connection.wait_for(lk, std::chrono::seconds(1)); // to avoid cpu burst
      continue;
    }

    cv_connection.wait_for(lk,
                           std::chrono::seconds(SERVER_ACCEPT_TIMEOUT_SECONDS),
                           [this] { return clientConnection.load(); });
    clientInfo client_info{.client_socket = 1, .client_ip = "localhost"};

    SimpleLogger::LOG_INFO(client_info.str(), " Client connection.");

    clientHandlers_.emplace_back(
        [this, client_info](std::stop_token st) {
          handle_client(client_info, st);
        },
        stopSource_.get_token());

    clientIsConnected = true;
  }
}

void SimpleHTTPServerMock::stop() {
  stopSource_.request_stop();
  isStarted_ = false;
}

void SimpleHTTPServerMock::handle_client(const clientInfo &client_info,
                                         const std::stop_token &stoken) {
  std::string lineBuffer;
  std::unique_lock lk(cv_m);

  while (!stoken.stop_requested()) {
    SimpleLogger::LOG_INFO(client_info.str(),
                           "MOCK TEST - SERVER WAITING FOR DATA");
    cv_data.wait_for(lk, std::chrono::seconds(CLIENT_RECV_TIMEOUT_SECONDS),
                     [this] { return clientIsSendingData.load(); });

    clientIsSendingData = false;

    while (!recvQueue.empty()) {
      auto bufferQueueElement = recv_read();
      size_t bytesReceived = bufferQueueElement.size();

      lineBuffer.append(bufferQueueElement.c_str(), bytesReceived + 1);
      SimpleLogger::LOG_INFO(
          client_info.str(),
          "MOCK TEST - SERVER DATA PROCESSING: ", lineBuffer);
      processLineBuffer(lineBuffer, client_info);
    }
  }
  SimpleLogger::LOG_INFO(client_info.str(),
                         SimpleLang::Message(Message::TCPClientDisconnected));
  clientConnection = false;
  clientIsConnected = false;
}

void SimpleHTTPServerMock::processLine(const std::string &line,
                                       const clientInfo &client_info) {
  if (threadMutex_.try_lock_for(std::chrono::seconds(MUTEX_TIMEOUT_SECONDS))) {
    try {
      if (smlp::trimALL(line).empty()) {
        return;
      }
      auto &manager = Manager::getInstance();
      auto &app_params = manager.app_params;
      if (app_params.verbose) {
        SimpleLogger::LOG_INFO(client_info.str(),
                               "[RECV FROM CLIENT: REQUEST] ", line);
      }
      // parsing
      const auto &request = parseHttpRequest(line);

      // validation
      const auto &validation = httpRequestValidation(request);
      if (!validation.isSuccess()) {
        SimpleLogger::LOG_ERROR(client_info.str(), validation.message());
        const auto &httpResponseInvalid = buildHttpResponse(validation);
        send_write(httpResponseInvalid.c_str());
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
      send_write(httpResponse.c_str());

    } catch (std::exception &ex) {
      SimpleLogger::LOG_ERROR(client_info.str(), ex.what());
      const auto &httpResponse = buildHttpResponse(ex);
      send_write(httpResponse.c_str());
      threadMutex_.unlock();
    }
    threadMutex_.unlock();
  }
}