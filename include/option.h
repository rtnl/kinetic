#pragma once

#include "unit.h"
#include "either.h"
#include <stdexcept>

namespace kinetic {

template <typename T>
class Option {
private:
  using Inner = Either<Unit, T>;

  Inner _inner;

  explicit Option(Inner inner)
    : _inner(std::move(inner))
  {}

public:
  static Option some(T value) {
    return Option(Inner(value));
  }

  static Option none() {
    return Option(Inner(Unit()));
  }

  bool is_none() const {
    return _inner.get_is_left();
  }

  bool is_some() const {
    return _inner.get_is_right();
  }

  T unwrap() const {
    if (is_none()) {
      throw std::logic_error("Option is None");
    }

    return _inner.get_r();
  }
};

}
