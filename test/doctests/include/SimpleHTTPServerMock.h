#pragma once
#include "EventMediator.h"
#include "SimpleHTTPServer.h"
#include "SimpleLogger.h"
#include <SimpleLang.h>
#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <thread>

class SimpleHTTPServerMock : public SimpleHTTPServer {
public:
  using SimpleHTTPServer::SimpleHTTPServer;

  std::shared_ptr<EventMediator> mediator = nullptr;

  void notifyClient(const Event &event) const {
    if (mediator == nullptr) {
      return;
    }
    mediator->pushClientEvent(event);
  }

  void notifyClient(const Event::Type &type,
                    const std::string &data = "") const {
    notifyClient(Event(type, data));
  }

  void start() override;

  void stop() override;

  void handle_client(const clientInfo &client_info,
                     const std::stop_token &stoken) override;

  void processLine(const std::string &line,
                   const clientInfo &client_info) override;
};