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

  constexpr T& operator[](const size_t index) const {
    return _source.get()[_start + index];
  }

  const T& at(const size_t index) const {
    if ((_start + index) >= get_source_len()) {
      return std::out_of_range("Slice::at out of range");
    }

    return _source.get()[index];
  }

  template <typename S>
  S pull(const size_t start, const size_t len) const {
    return S(Slice(this->_source, start, len));
  }

  template <typename S>
  S pull_offset(const size_t start, const size_t len) const {
    return pull<S>(_start + start, _len + len);
  }
};

}
