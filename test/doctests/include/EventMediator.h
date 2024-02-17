#pragma once
#include "Event.h"
#include <condition_variable>
#include <queue>

class EventMediator {
public:
  std::atomic<bool> is_client_connected = false;

  void pushServerEvent(const Event &event) {
    std::lock_guard lock(server_mtx);
    serverEventQueue.push(event);
    server_cv.notify_all();
  }

  Event popServerEvent() {
    std::unique_lock lock(server_mtx);
    server_cv.wait(lock, [this] { return !serverEventQueue.empty(); });
    Event event = serverEventQueue.front();
    serverEventQueue.pop();
    return event;
  }

  void pushClientEvent(const Event &event) {
    std::lock_guard lock(client_mtx);
    clientEventQueue.push(event);
    client_cv.notify_all();
  }

  Event popClientEvent() {
    std::unique_lock lock(client_mtx);
    client_cv.wait(lock, [this] { return !clientEventQueue.empty(); });
    Event event = clientEventQueue.front();
    clientEventQueue.pop();
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