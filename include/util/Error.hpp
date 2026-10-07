#pragma once

#include "../prelude.hpp"

namespace KINETIC_NAMESPACE {

enum class ErrorKind {
  ValueNone,
  ValueUnavailable,
  PlatformError,
  PlatformUnimplemented,
  IoEnded,
};

class Error {
private:
  ErrorKind _kind;

  std::string _message;

public:
  Error(const ErrorKind kind, const std::string message)
    : _kind(kind)
    , _message(message)
  {}

  ErrorKind get_kind() const {
    return _kind;
  }

  const std::string & get_message() const {
    return _message;
  }
};

}
