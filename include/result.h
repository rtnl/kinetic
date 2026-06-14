#pragma once

#include <stdexcept>
#include <string>

#include "error.h"
#include "either.h"

namespace kinetic {

template <typename T>
class Result {
private:
  using Inner = Either<Error, T>;

  Inner _inner;

  explicit Result(Inner inner) noexcept
    : _inner(std::move(inner))
  {}

public:
  static Result ok(const T & value) noexcept {
    return Result(Inner(std::move(value)));
  }

  static Result err(Error error) noexcept {
    return Result(Inner(error));
  }

  static Result err(ErrorKind error_kind, const std::string & error_msg) noexcept {
    return Result(Inner(Error(error_kind, error_msg)));
  }

  bool is_ok() const {
    return _inner.get_is_right();
  }

  bool is_err() const {
    return _inner.get_is_left();
  }

  Error get_error() const {
    if (is_ok()) {
      return Error(ErrorKind::None, "");
    } else {
      return _inner.get_l();
    }
  }

  const T & unwrap() const {
    if (is_err()) {
      throw std::logic_error("Result is Error: " + get_error().formatted());
    }

    return _inner.get_r();
  }
};

}
