/**
 * @file Monitor.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Simple monitoring
 * @date 2023-09-08
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class Monitor {
public:
  explicit Monitor(const std::string &logFileName) {
    logFile_.open(logFileName,
                  std::fstream::in | std::fstream::out | std::fstream::app);
    if (!logFile_.is_open()) {
      std::cerr << "Failed to open log file: " << logFileName << std::endl;
    }
  }

  ~Monitor() {
    if (logFile_.is_open()) {
      logFile_.close();
    }
  }

  void eol() {
    if (logFile_.is_open()) {
      logFile_ << std::endl;
    }
  }

  void log(const std::string &value, bool isLast = false) {
    if (logFile_.is_open()) {
      logFile_ << value;
      if (!isLast) {
        logFile_ << ",";
      } else {
        logFile_ << std::endl;
      }
    }
  }

  void log(size_t value, bool isLast = false) {
    if (logFile_.is_open()) {
      logFile_ << value;
      if (!isLast) {
        logFile_ << ",";
      } else {
        logFile_ << std::endl;
      }
    }
  }

  void log(int value, bool isLast = false) {
    if (logFile_.is_open()) {
      logFile_ << value;
      if (!isLast) {
        logFile_ << ",";
      } else {
        logFile_ << std::endl;
      }
    }
  }

  void log(float value, bool isLast = false) {
    if (logFile_.is_open()) {
      logFile_ << std::setprecision(2) << value;
      if (!isLast) {
        logFile_ << ",";
      } else {
        logFile_ << std::endl;
      }
    }
  }

private:
  std::ofstream logFile_;
};