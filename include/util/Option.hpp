#pragma once

#include "../prelude.hpp"
#include "./Either.hpp"
#include "./Unit.hpp"

namespace KINETIC_NAMESPACE {

template <typename A>
class Option {
private:
  Either<A, Unit> _inner;

public:
  Option()
    : _inner(Unit())
  {}

  Option(const A & value)
    : _inner(value)
  {}

  bool is_some() const {
    return _inner.is_l();
  }

  bool is_none() const {
    return _inner.is_r();
  }

  const A & get() const {
    assert(is_some());
    return _inner.get_l();
  }

  const A & unwrap() const {
    if (is_none()) {
      throw std::runtime_error("Option has no value");
    }

    return _inner.get_l();
  }
};

}
