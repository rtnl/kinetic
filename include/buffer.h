#pragma once

#include <string>
#include <vector>

#include "type.h"

namespace kinetic {

class ByteBuffer {
private:
  std::vector<u8> _inner;

public:
  ByteBuffer()
    : _inner({})
  {}

  const size_t size() const {
    return _inner.size();
  }

  const u8 * raw() const {
    return _inner.data();
  }

  size_t write(const u8 * source, const size_t len) {
    _inner.insert(_inner.end(), source, &source[len]);

    return len;
  }

  size_t write(const char * source, const size_t len) {
    return write(reinterpret_cast<const u8 *>(source), len);
  }

  size_t write(const std::string & source) {
    return write(source.c_str(), strlen(source.c_str()));
  }
};

}
