#pragma once

#include "../prelude.hpp"
#include "./Either.hpp"
#include "./Error.hpp"

namespace KINETIC_NAMESPACE {

template <typename A>
class Result {
private:
  Either<A, Error> _inner;

public:
  Result(const A & value)
    : _inner(value)
  {}

  Result(const Error & error)
    : _inner(error)
  {}

  Result(const ErrorKind error_kind, const std::string & error_msg)
    : _inner(Error(error_kind, error_msg))
  {}

  bool is_ok() const {
    return _inner.is_l();
  }

  bool is_err() const {
    return _inner.is_r();
  }

  const A & get() const {
    assert(is_ok());
    return _inner.get_l();
  }

  const A & unwrap() const {
    if (is_err()) {
      throw std::runtime_error("Result is error");
    }

    return _inner.get_l();
  }
};

}
