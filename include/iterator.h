#pragma once

#include <memory>
#include <vector>

#include "option.h"

namespace kinetic {
// Todo: make streams?
template <typename T>
class Iterator { // Todo: implement c++ iterator?
public:
  virtual bool has_next() = 0;

  virtual Option<T> get_next() = 0;
};

template <typename T>
class VectorIterator : public Iterator<T> {
private:
  std::shared_ptr<std::vector<T>> _source;

  size_t _index;

  size_t get_index_then_increment() {
    const auto result = _index;

    _index++;

    return result;
  }

public:
  VectorIterator(std::shared_ptr<std::vector<T>> source)
    : _source(source)
    , _index(0)
  {}

  bool has_next() {
    return _index < _source.get()->size();
  }

  Option<T> get_next() {
    if (!has_next()) {
      return Option<T>::none();
    }

    return Option<T>::some(_source.get()->at(get_index_then_increment()));
  }
};

}
