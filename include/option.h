#pragma once

#include "unit.h"
#include "either.h"

#include <stdexcept>
#include <functional>

namespace kinetic {

template<typename T>
class Option;

template<typename T>
struct is_option : std::false_type {};

template<typename U>
struct is_option<Option<U>> : std::true_type {};

template <typename T>
class Option {
private:
  using Inner = Either<Unit, T>;

  Inner _inner;

  explicit Option(Inner inner)
    : _inner(std::move(inner))
  {}

public:
  Option()
    : _inner(Inner(Unit()))
  {}

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

  T unwrap_or(const T & fallback) const {
    if (is_none()) {
      return fallback;
    }

    return _inner.get_r();
  }

  template <typename U>
  Option<U> flatmap(const std::function<Option<U> (const T &)> & fn) const {
    if (is_none()) {
      return Option<U>::none();
    }

    return fn(_inner.get_r());
  }

  template <typename U>
  Option<U> map(const std::function<U (const T &)> & fn) const {
    if (is_none()) {
      return Option<U>::none();
    }

    return Option<U>::some(fn(_inner.get_r()));
  }

  // enabled only if T == Option<U>
  template<
    typename X = T,
    typename std::enable_if<is_option<X>::value, int>::type = 0
  >
  X flatten() const {
    if (!is_some()) {
      return X::none();
    }

    return _inner.get_r();
  }
};

}
