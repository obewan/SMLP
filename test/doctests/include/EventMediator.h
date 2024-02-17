#pragma once
#include "Event.h"
#include "SimpleLogger.h"
#include <condition_variable>
#include <queue>

class EventMediator {
public:
  std::atomic<bool> is_client_connected = false;

  void pushServerEvent(const Event &event) {
    std::lock_guard lock(server_mtx);
    // SimpleLogger::LOG_INFO("Push server event (", (int)event.getType(),
    //                        "): ", event.getData());
    serverEventQueue.push(event);
    server_cv.notify_all();
  }

  Event popServerEvent() {
    std::unique_lock lock(server_mtx);
    // SimpleLogger::LOG_INFO("Pop server event...");
    server_cv.wait(lock, [this] { return !serverEventQueue.empty(); });
    Event event = serverEventQueue.front();
    serverEventQueue.pop();
    // SimpleLogger::LOG_INFO("Pop server event ok");
    return event;
  }

  void pushClientEvent(const Event &event) {
    std::lock_guard lock(client_mtx);
    // SimpleLogger::LOG_INFO("Push client event (", (int)event.getType(),
    //                        "): ", event.getData());
    clientEventQueue.push(event);
    client_cv.notify_all();
  }

  Event popClientEvent() {
    std::unique_lock lock(client_mtx);
    // SimpleLogger::LOG_INFO("Pop client event...");
    client_cv.wait(lock, [this] { return !clientEventQueue.empty(); });
    Event event = clientEventQueue.front();
    clientEventQueue.pop();
    // SimpleLogger::LOG_INFO("Pop client event ok");
    return event;
  }

private:
  std::queue<Event> serverEventQueue;
  std::queue<Event> clientEventQueue;
  std::mutex server_mtx;
  std::condition_variable server_cv;
  std::mutex client_mtx;
  std::condition_variable client_cv;
};