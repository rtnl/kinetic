#pragma once

#include "type.h"
#include "meta.h"
#include "unit.h"
#include "error.h"
#include "result.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iterator>
#include <vector>
#include <memory>
#include <istream>
#include <fstream>
#include <iostream>

namespace kinetic {

static const size_t READER_BUFLEN = 8192;

using R_Size = Result<usize>;
using R_Unit = Result<Unit>;

class Reader {
public:
  virtual ~Reader() = default;

  virtual R_Size read(u8 * buf, usize size) = 0;

  R_Size read_to_end(std::vector<u8> & out) {
    size_t len = 0;

    u8 buf[READER_BUFLEN];

    while (true) {
      const R_Size read_res = read(buf, READER_BUFLEN);
      if (read_res.is_err()) {
        if (read_res.get_error().get_kind() == ErrorKind::ValueEnded) {
          break;
        }

        return R_Size::err(read_res.get_error());
      }

      const size_t read_len = read_res.unwrap();
      out.insert(out.end(), std::begin(buf), &buf[read_len]);

      len += read_len;
    }

    return R_Size::ok(len);
  }

  R_Unit read_exact(u8 * out, usize size) {
    usize s_read = 0;

    while (s_read < size) {
      const auto read_r = read(&out[s_read], size - s_read);
      if (read_r.is_err()) {
        return R_Unit::err(read_r.get_error());
      }

      const usize l = read_r.unwrap();
      if (l < 1) {
        return R_Unit::err(kinetic::ErrorKind::ValueInvalid, "read_exact zero read");
      }

      s_read += l;
    }

    return R_Unit::ok({});
  }
};

class BufReader : public Reader {
private:
  u8 _buf[READER_BUFLEN] = {0};

  size_t _buf_len = 0;

public:
  ~BufReader() = default;

  R_Size read_until(u8 value, std::vector<u8> & out) {
    size_t written = 0;

    while (true) {
      if (_buf_len == 0) {
        const R_Size read_res = read(_buf, READER_BUFLEN);
        if (read_res.is_err()) {
          return read_res;
        }

        _buf_len = read_res.unwrap();
      }

      if (_buf_len == 0) {
        return R_Size::ok(written);
      }

      const u8     * match = reinterpret_cast<u8 *>(std::memchr(_buf, value, _buf_len));
      const u8     * limit = (match == nullptr) ? nullptr  : (match + 1);
      const size_t   len_l = (match == nullptr) ? _buf_len : limit - _buf;
      const size_t   len_r = (match == nullptr) ? 0        : _buf_len - len_l;

      out.insert(out.end(), &_buf[0], &_buf[len_l]);
      written += len_l;

      for (size_t x = 0; x < len_r; x++) {
        _buf[x] = limit[x];
      }
      _buf_len = len_r;

      if (match) {
        return R_Size::ok(written);
      }
    }
  }
};

class StreamReader : public BufReader {
private:
  std::unique_ptr<std::istream> _stream;

public:
  ~StreamReader() = default;

  StreamReader(std::unique_ptr<std::istream> stream) noexcept
    : _stream(std::move(stream))
  {
    _stream->unsetf(std::ios::skipws);
  }

  R_Size read(u8 * buf, const usize size) override {
    if (buf == nullptr) {
      return R_Size::err(ErrorKind::ValueNull, "buf is null");
    }

    if (size == 0) {
      return R_Size::ok(0);
    }

    if (!_stream) {
      return R_Size::err(ErrorKind::ValueInvalid, "stream already failed");
    }

    if (!_stream->good()) {
      return R_Size::err(ErrorKind::ValueInvalid, "stream is not good");
    }

    _stream->read(reinterpret_cast<char *>(buf), static_cast<std::streamsize>(size));
    const size_t read_len = _stream->gcount();

    return R_Size::ok(read_len);
  }
};

class FileReader : public StreamReader {
public:
  FileReader(const std::string & path)
    : StreamReader(std::unique_ptr<std::ifstream>(new std::ifstream(path, std::ios::binary)))
  {}
};

}
