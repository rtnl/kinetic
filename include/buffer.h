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

  size_t read(u8 * destination, const size_t len) const {
    for (size_t x = 0; x < len; x++) {
      destination[x] = _inner[x];
    }

    return len;
  }

  size_t read_string(std::string & destination) const {
    bool flag_null_terminated = false;
    size_t index_end = 0;

    for (size_t x = 0; x < size(); x++) {
      if (_inner[x] == 0x00) {
        flag_null_terminated = true;
        index_end = x;
        break;
      }
    }

    if (!flag_null_terminated) {
      return 0;
    }

    const std::string s((char *) raw(), index_end);

    destination.append(s);

    return index_end;
  }
};

}
