#pragma once

#include "../_prelude.hpp"
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

  const Error & get_error() const {
    assert(is_err());
    return _inner.get_r();
  }

  const A & unwrap() const {
    if (is_err()) {
      throw std::runtime_error("Result is error");
    }

    return _inner.get_l();
  }

  template <typename B>
  Result<B> map(const std::function<B (const A &)> & fn_map) const {
    if (is_err()) {
      return {get_error()};
    }

    return {fn_map(get())};
  }

  template <typename B>
  Result<B> operator|(const std::function<B (const A &)> & fn_map) const {
    return map(fn_map);
  }

  template <typename B>
  Result<B> flatmap(const std::function<Result<B> (const A &)> & fn_flatmap) const {
    if (is_err()) {
      return {get_error()};
    }

    return fn_flatmap(get());
  }

  template <typename B>
  Result<B> operator|(const std::function<Result<B> (const A &)> & fn_flatmap) const {
    return flatmap(fn_flatmap);
  }

};

}
