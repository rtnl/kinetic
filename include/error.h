#pragma once

#include <string>
#include <utility>
#include <sstream>

#include "meta.h"

namespace kinetic {

enum class ErrorKind {
  None,
  ValueNull,
  ValueNotInitialized,
  ValueNotAvailable,
  ValueInvalid,
  ValueEnded,
  SourceTodo,
};

class Error {
private:
  ErrorKind _kind;

  std::string _message;

public:
  Error()
    : _kind(ErrorKind::None)
    , _message("")
  {}

  Error(ErrorKind kind, std::string message)
    : _kind(kind)
    , _message(std::move(message))
  {}

  KINETIC_GETTER(_kind, kind)

  KINETIC_GETTER(_message, message)

  std::string formatted() const {
    std::ostringstream ss;

    ss
      << "Error"
      << "("
      << int(get_kind())
      << "|"
      << get_message()
      << ")";

    return ss.str();
  }
};

}
