#pragma once

#include <memory>
#include <type_traits>

#include "meta.h"
#include "error.h"
#include "option.h"
#include "result.h"
#include "iterator.h"

namespace kinetic {

template <typename I, typename R>
class ParserState {
public:
  virtual ~ParserState() = default;

  virtual kinetic::Result<std::shared_ptr<R>> apply(const std::shared_ptr<R> & state, const I & input) = 0;
};

template <typename I, typename R, typename S>
class Parser {
  static_assert(std::is_base_of<ParserState<I, R>, S>::value, "S must extend ParserState<I, R>");

private:
  std::shared_ptr<kinetic::Iterator<I>> _iter;

  std::shared_ptr<S> _state;

  std::shared_ptr<R> _value;

  Result<std::shared_ptr<R>> _result;

  size_t _count;

public:
  Parser(std::shared_ptr<kinetic::Iterator<I>> iter, std::shared_ptr<S> state, std::shared_ptr<R> init)
    : _iter(std::move(iter))
    , _state(std::move(state))
    , _value(std::move(init))
    , _result(Result<std::shared_ptr<R>>::err(Error(ErrorKind::ValueNotInitialized, "Parser did not step")))
    , _count(0)
  {}

  KINETIC_GETTER(_count, count)

  KINETIC_GETTER(_result, result)

  bool step() {
    if (_state == nullptr) {
      _result = Result<std::shared_ptr<R>>::err(Error(ErrorKind::ValueNull, "Parser init value is null"));
      return false;
    }

    const kinetic::Option<I> element_r = _iter->get_next();
    if (element_r.is_none()) {
      _result = Result<std::shared_ptr<R>>::ok(_value);
      return false;
    }

    const I element = element_r.unwrap();

    Result<std::shared_ptr<R>> value_result = _state->apply(_value, element);
    if (value_result.is_err()) {
      _result = value_result;
      return false;
    }

    _value = value_result.unwrap();
    _count++;

    return true;
  }

  kinetic::Result<std::shared_ptr<R>> run() {
    while (step()) {
    }

    return get_result();
  }
};

}
