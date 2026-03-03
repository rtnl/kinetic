#pragma once

#include <memory>
#include <type_traits>

#include "meta.h"
#include "error.h"
#include "result.h"
#include "iterator.h"

namespace kinetic {

template <typename I, typename R>
class ParserState {
public:
  virtual ~ParserState() = default;

  virtual kinetic::Result<std::shared_ptr<R>> apply(std::shared_ptr<R> state, const I & input) = 0;
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
    , _state(state)
    , _value(init)
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

    if (!_iter->has_next()) {
      _result = Result<std::shared_ptr<R>>::ok(_value);
      return false;
    }

    const kinetic::Result<I> element_r = _iter->get_next();
    if (element_r.is_err()) {
      _result = Result<std::shared_ptr<R>>::err(Error(ErrorKind::ValueNotAvailable, "failed to get next item from iterator: " + element_r.get_error()));
      return false;
    }

    const I element = element_r.unwrap();

    Result<std::shared_ptr<R>> value_result = _state->apply(_value, element);
    if (value_result.is_err()) {
      _result = value_result;
      return false;
    }

    _value = value_result.unwrap();
    return true;
  }

  kinetic::Result<std::shared_ptr<R>> run() {
    while (step()) {
      _count++;
    }

    return get_result();
  }
};

}
