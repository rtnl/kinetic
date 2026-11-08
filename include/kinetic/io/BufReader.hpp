#pragma once

#include "../_prelude.hpp"
#include "./Config.hpp"
#include "./BufRead.hpp"
#include "../util/Linear.hpp"

namespace KINETIC_NAMESPACE {

class BufReader : public BufRead {
private:
  std::shared_ptr<Read> _reader;

  std::array<u8, KINETIC_BUFFER_SIZE> _buffer;

  usize _buffer_l;

  usize _buffer_r;

public:
  BufReader(const std::shared_ptr<Read> & reader)
    : _reader(reader)
    , _buffer()
    , _buffer_l()
    , _buffer_r()
  {}

  ~BufReader() override = default;

  bool is_buffer_available() const {
    return _buffer_r > _buffer_l;
  }

  usize get_buffer_remaining() const {
    return _buffer_r - _buffer_l;
  }

  Result<usize> fill_buf() override {
    if (_reader == nullptr) {
      return {ErrorKind::ValueNone, "reader is null"};
    }

    if (is_buffer_available()) {
      return {get_buffer_remaining()};
    }

    const auto io_r = _reader->read(&_buffer[0], KINETIC_BUFFER_SIZE);
    if (io_r.is_err()) {
      return {io_r.get_error()};
    }

    _buffer_l = 0;
    _buffer_r = io_r.get();
    return {_buffer_r};
  }

  void consume(const usize size) override {
    _buffer_l += (std::min)(_buffer_r - _buffer_l, size);
  }

  Result<usize> read(u8 * into, const usize limit) override {
    if (into == nullptr) {
      return {ErrorKind::ValueNone, "into is null"};
    }

    if (limit == 0) {
      return {0};
    }

    if (!is_buffer_available()) {
      const auto fill_r = fill_buf();
      if (fill_r.is_err()) {
        return {fill_r.get_error()};
      }
      if (fill_r.get() == 0) {
        return {0};
      }
    }

    const usize copied = (std::min)(get_buffer_remaining(), limit);
    std::memcpy(into, &_buffer[_buffer_l], copied);
    consume(copied);
    return {copied};
  }

  // Todo: incorrect when delimiter comes at the boundary of a read
  Result<usize> read_until(const u8 * bytes, const usize size, std::vector<u8> & out) override {
    usize copied = 0;

    if (bytes == nullptr || size == 0) {
      return {0};
    }

    for (;;) {
      if (!is_buffer_available()) {
        const auto fill_r = fill_buf();
        if (fill_r.is_err()) {
          return {fill_r.get_error()};
        }
        if (fill_r.get() == 0) {
          return {copied};
        }
      }

      const auto remaining = get_buffer_remaining();

      const auto x_opt = Linear::find(
        const_cast<const u8 *>(&_buffer[_buffer_l]),
        remaining,
        &bytes[0],
        size);
      if (x_opt.is_none()) {
        out.insert(out.end(), &_buffer[_buffer_l], &_buffer[_buffer_r]);
        consume(remaining);
        copied += remaining;
        continue;
      }

      const auto chunk = x_opt.get() + size;
      out.insert(out.end(), &_buffer[_buffer_l], &_buffer[_buffer_l + chunk]);
      consume(chunk);
      copied += chunk;
      break;
    }

    return {copied};
  }

  Result<usize> skip_until(const u8 *bytes, const usize size) override {
    return {{}};
  }
};

}
