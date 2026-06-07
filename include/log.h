#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <mutex>
#include <./color.h>

namespace kinetic {

enum class LogLevel {
  Trace,
  Debug,
  Info,
  Warn,
  Error,
  Fatal,
  Test,
};

static std::string Display(const LogLevel level) {
  switch (level) {
    case LogLevel::Trace: return "trace";
    case LogLevel::Debug: return "debug";
    case LogLevel::Info:  return "info";
    case LogLevel::Warn:  return "warn";
    case LogLevel::Error: return "error";
    case LogLevel::Fatal: return "fatal";
    case LogLevel::Test:  return "test";
    default:
      throw std::runtime_error("unimplemented");
  }
}

static std::string Display_padded(const LogLevel level) {
  switch (level) {
    case LogLevel::Trace: return "trace";
    case LogLevel::Debug: return "debug";
    case LogLevel::Info:  return "info ";
    case LogLevel::Warn:  return "warn ";
    case LogLevel::Error: return "error";
    case LogLevel::Fatal: return "fatal";
    case LogLevel::Test:  return "test ";
    default:
      throw std::runtime_error("unimplemented");
  }
}

static ColorCode LogLevel_color(const LogLevel level) {
  switch (level) {
    case LogLevel::Trace: return ColorCode::Magenta;
    case LogLevel::Debug: return ColorCode::Cyan;
    case LogLevel::Info:  return ColorCode::White;
    case LogLevel::Warn:  return ColorCode::Yellow;
    case LogLevel::Error: return ColorCode::Red;
    case LogLevel::Fatal: return ColorCode::Red;
    case LogLevel::Test:  return ColorCode::Green;
    default:
      throw std::runtime_error("unimplemented");
  }
}

class Log {
private:
  std::mutex _io_m;

  std::mutex _state_m;

  std::string _id;

public:
  Log(const std::string & id)
    : _id(id)
  {}

  void set_id(const std::string & id) {
    std::lock_guard<std::mutex> l(_state_m);

    _id = id;
  }

  std::string get_id() {
    std::lock_guard<std::mutex> l(_state_m);

    return _id;
  }

  void print(const LogLevel level, const std::string & message) {
    std::lock_guard<std::mutex> l_state(_state_m);
    std::lock_guard<std::mutex> l_io(_io_m);

    std::ostringstream ss;

    ss << "#" << Display_padded(level) << " | "
       << "@" << _id << " | "
       << "$" << '"' << message << '"' << std::endl;

    ColorCode color_code = LogLevel_color(level);

    std::cerr << ColorCode_wrap(ss.str(), color_code);
  }

  void Trace(const std::string & message) {
    print(LogLevel::Trace, message);
  }

  void Debug(const std::string & message) {
    print(LogLevel::Debug, message);
  }

  void Info(const std::string & message) {
    print(LogLevel::Info, message);
  }

  void Warn(const std::string & message) {
    print(LogLevel::Warn, message);
  }

  void Error(const std::string & message) {
    print(LogLevel::Error, message);
  }

  void Fatal(const std::string & message) {
    print(LogLevel::Fatal, message);
  }

  void Test(const std::string & message) {
    print(LogLevel::Test, message);
  }
};

static Log _LOG_DEFAULT("rtnl.cloud:kinetic");

}
