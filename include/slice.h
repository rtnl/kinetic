#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

namespace kinetic {

template <typename T>
class Slice {
private:
  std::shared_ptr<std::vector<T>> _source;

  size_t _start;

  size_t _len;

protected:
  const std::vector<T> * get_source() const noexcept {
    return _source.get();
  }

  Slice(const Slice<T> &other)
    : _source(other._source)
    , _start(other._start)
    , _len(other._len)
  {}

public:
  Slice(std::shared_ptr<std::vector<T>> source, size_t start, size_t len)
    : _source(source)
    , _start(start)
    , _len(len)
  {}

  Slice(std::shared_ptr<std::vector<T>> source)
    : _source(source)
    , _start(0)
    , _len(source.get()->size())
  {}

  size_t get_source_len() const noexcept {
    return _source.get()->size();
  }

  size_t get_start() const {
    return _start;
  }

  size_t get_len() const {
    return _len;
  }

  bool check() const {
    if (_start >= get_source_len()) {
      return false;
    }

    if ((_start + _len) >= get_source_len()) {
      return false;
    }

    return true;
  }

  const T& operator[](const size_t index) const {
    return (*_source)[_start + index];
  }

  const T& at(const size_t index) const {
    if ((_start + index) >= get_source_len()) {
      throw std::out_of_range("Slice::at out of range");
    }

    return (*_source)[_start + index]; 
  }

  template <typename S>
  S pull(const size_t start, const size_t len) const {
    return S(Slice(this->_source, start, len));
  }

  template <typename S>
  S pull_offset(const size_t start, const size_t len) const {
    return pull<S>(_start + start, _len + len);
  }

  bool has_prefix(const Slice<T> & other) const {
    if (other.get_len() >= get_len()) {
      return false;
    }

    for (size_t x = 0; x < other.get_len(); x++) {
      if (at(x) != other.at(x)) {
        return false;
      }
    }

    return true;
  }

  bool has_suffix(const Slice<T> & other) const {
    if ((get_start() + other.get_len()) >= get_source_len()) {
      return false;
    }

    for (size_t x = 0; x < other.get_len(); x++) {
      if (at(get_start() + get_len() - other.get_len() + x) != other.at(x)) {
        return false;
      }
    }

    return true;
  }
};

}
