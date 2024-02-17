#include "SimpleHTTPServerMock.h"
#include "Event.h"
#include "Manager.h"
#include "SimpleLogger.h"
#include "SimpleTCPClientMock.h"
#include <chrono>

constexpr int MUTEX_TIMEOUT_SECONDS = 60;

void SimpleHTTPServerMock::start() {
  if (mediator == nullptr) {
    return;
  }
  if (bool expected = false; !isStarted_.compare_exchange_strong(
          expected, true)) { // thread safe comparison with exchange
    return;
  }

  notifyClient(Event::Type::ServerStarted);

  while (!stopSource_.stop_requested()) {
    if (mediator->is_client_connected) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }

    notifyClient(Event::Type::ServerListening);
    const auto &event = mediator->popServerEvent();
    if (event.getType() != Event::Type::ClientConnection) {
      SimpleLogger::LOG_ERROR(
          "MOCK TEST - SERVER RECEIVING A NON CONNECTION EVENT (",
          (int)event.getType(), ")");
      return;
    }

    clientInfo client_info{.client_socket = 1, .client_ip = "localhost"};

    SimpleLogger::LOG_INFO(client_info.str(), " Client connection.");

    clientHandlers_.emplace_back(
        [this, client_info](std::stop_token st) {
          handle_client(client_info, st);
        },
        stopSource_.get_token());

    mediator->is_client_connected = true;
    notifyClient(Event::Type::ClientConnected);
  }
}

void SimpleHTTPServerMock::stop() {
  stopSource_.request_stop();
  isStarted_ = false;
}

void SimpleHTTPServerMock::handle_client(const clientInfo &client_info,
                                         const std::stop_token &stoken) {
  std::string lineBuffer;
  while (!stoken.stop_requested() && mediator->is_client_connected) {
    SimpleLogger::LOG_INFO(client_info.str(),
                           "MOCK TEST - SERVER WAITING FOR DATA...");
    const auto &event = mediator->popServerEvent();

    if (event.getType() == Event::Type::ClientDisconnection) {
      mediator->is_client_connected = false;
      continue;
    } else if (event.getType() != Event::Type::Message) {
      SimpleLogger::LOG_ERROR(
          "MOCK TEST - SERVER RECEIVING A NON MESSAGE EVENT (",
          (int)event.getType(), ")");
      continue;
    }
    const auto &data = event.getData();
    lineBuffer.append(data);
    SimpleLogger::LOG_INFO(client_info.str(),
                           "MOCK TEST - SERVER DATA PROCESSING: ", lineBuffer);
    processLineBuffer(lineBuffer, client_info);
  }
  SimpleLogger::LOG_INFO(client_info.str(),
                         SimpleLang::Message(Message::TCPClientDisconnected));
}

void SimpleHTTPServerMock::processLine(const std::string &line,
                                       const clientInfo &client_info) {
  if (threadMutex_.try_lock_for(std::chrono::seconds(MUTEX_TIMEOUT_SECONDS))) {
    try {
      if (smlp::trimALL(line).empty()) {
        threadMutex_.unlock();
        return;
      }
      auto &manager = Manager::getInstance();
      const auto &app_params = manager.app_params;
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
        notifyClient(Event::Type::Message, httpResponseInvalid);
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
      notifyClient(Event::Type::Message, httpResponse);

    } catch (std::exception &ex) {
      SimpleLogger::LOG_ERROR(client_info.str(), ex.what());
      const auto &httpResponse = buildHttpResponse(ex);
      notifyClient(Event::Type::Message, httpResponse);
      threadMutex_.unlock();
    }
    threadMutex_.unlock();
  }
}