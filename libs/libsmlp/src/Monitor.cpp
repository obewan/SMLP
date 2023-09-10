#include "Monitor.h"
#include <ctime>
#include <iomanip>

void Monitor::eol() {
  if (logFile_.is_open()) {
    logFile_ << std::endl;
  }
}

void Monitor::date(time_point_t timepoint, bool isLast) {
  if (logFile_.is_open()) {
    auto timet = std::chrono::system_clock::to_time_t(timepoint);

    // Gets the milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timepoint.time_since_epoch());
    auto milliseconds = ms.count() % 1000;

    // Convert time_t to std::tm
    std::tm tm_time;
    localtime_r(&timet, &tm_time);

    // Format the time
    logFile_ << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S") << "."
             << std::setw(3) << std::setfill('0') << milliseconds;

    if (!isLast) {
      logFile_ << ",";
    } else {
      logFile_ << std::endl;
    }
  }
}

void Monitor::log(const std::string &value, bool isLast) {
  if (logFile_.is_open()) {
    logFile_ << value;
    if (!isLast) {
      logFile_ << ",";
    } else {
      logFile_ << std::endl;
    }
  }
}

void Monitor::log(size_t value, bool isLast) {
  if (logFile_.is_open()) {
    logFile_ << value;
    if (!isLast) {
      logFile_ << ",";
    } else {
      logFile_ << std::endl;
    }
  }
}

void Monitor::log(int value, bool isLast) {
  if (logFile_.is_open()) {
    logFile_ << value;
    if (!isLast) {
      logFile_ << ",";
    } else {
      logFile_ << std::endl;
    }
  }
}

void Monitor::log(float value, bool isLast) {
  if (logFile_.is_open()) {
    logFile_ << std::setprecision(2) << value;
    if (!isLast) {
      logFile_ << ",";
    } else {
      logFile_ << std::endl;
    }
  }
}
