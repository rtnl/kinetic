#pragma once

#include <string>
#include <utility>

#include "meta.h"

namespace kinetic {

enum class ErrorKind {
  None,
  ValueNull,
  ValueNotInitialized,
  ValueNotAvailable,
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
};

}
