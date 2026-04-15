#pragma once

#include <memory>
#include <vector>
#include <array>
#include <fstream>

#include "type.h"
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

  bool has_next() override {
    return _index < _source.get()->size();
  }

  Option<T> get_next() {
    if (!has_next()) {
      return Option<T>::none();
    }

    return Option<T>::some(_source.get()->at(get_index_then_increment()));
  }
};

class FileIterator : public Iterator<u8> {
private:
  static constexpr size_t _BUF_SIZE = 4096;

  std::string _path;

  std::ifstream _file;

  std::array<u8, _BUF_SIZE> _buffer;

  size_t _buffer_hold;

  size_t _buffer_read;

public:
  FileIterator(const std::string & path)
    : _path(std::move(path))
    , _file(path, std::ios::binary)
    , _buffer()
    , _buffer_hold(0)
    , _buffer_read(0)
  {}

  bool _load() {
    if (_buffer_read < _buffer_hold) {
      return true;
    }

    _file.read(reinterpret_cast<char *>(_buffer.data()), _BUF_SIZE);

    const auto n = _file.gcount();
    if (n < 1) {
      return false;
    }

    _buffer_hold = n;
    _buffer_read = 0;
    return true;
  }

  bool has_next() override {
    return _load();
  }

  Option<u8> get_next() override {
    if (!has_next()) {
      return Option<u8>::none();
    }

    return Option<u8>::some(_buffer[_buffer_read++]);
  }
};

}
