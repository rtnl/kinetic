#pragma once

#include <stdexcept>

#include "error.h"
#include "either.h"

namespace kinetic {

template <typename T>
class Result {
private:
  using Inner = Either<Error, T>;

  Inner _inner;

  explicit Result(Inner inner)
    : _inner(std::move(inner))
  {}

public:
  static Result ok(T value) {
    return Result(Inner(value));
  }

  static Result err(Error error) {
    return Result(Inner(error));
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

  T unwrap() const {
    if (is_err()) {
      throw std::logic_error("Result is Error: " + get_error().formatted());
    }

    return _inner.get_r();
  }
};

}
