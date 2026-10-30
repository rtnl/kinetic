#pragma once

#include "../_prelude.hpp"
#include "./Config.hpp"
#include "../util/Result.hpp"
#include "../util/Unit.hpp"

namespace KINETIC_NAMESPACE {

class Read {
  static_assert(KINETIC_BUFFER_SIZE > 0, "KINETIC_BUFFER_SIZE positive");

public:
  virtual ~Read() = default;

  virtual Result<usize> read(u8 * into, const usize size) = 0;

  Result<usize> read_to_end(std::vector<u8> & into) {
    usize size = 0;

    // Todo: we could copy directly into `into`
    std::array<u8, KINETIC_BUFFER_SIZE> buf;

    for (;;) {
      const Result<usize> io_r = read(buf.data(), KINETIC_BUFFER_SIZE);
      if (io_r.is_err()) {
        return io_r;
      }

      const usize io_n = io_r.get();
      if (io_n == 0) {
        break;
      }

      into.insert(into.end(), &buf[0], &buf[io_n]);
      size += io_n;
    }

    return {size};
  }

  Result<Unit> read_exact(u8 * into, const usize size) {
    std::array<u8, KINETIC_BUFFER_SIZE> buf;

    usize s_into = 0;
    usize s_left = size;

    if (into == nullptr) {
      return {ErrorKind::ValueNone, "into is null"};
    }
    if (size == 0) {
      return {{}};
    }

    while (s_left > 0) {
      const Result<usize> io_r = read(
        buf.data(),
        (std::min)(s_left, KINETIC_BUFFER_SIZE));
      if (io_r.is_err()) {
        return {io_r.get_error()};
      }

      const usize io_n = io_r.get();
      if (io_n == 0) {
        return {ErrorKind::IoEnded, "unexpected end"};
      }

      ::memcpy(&into[s_into], buf.data(), io_n);
      s_into += io_n;
      s_left -= io_n;
    }

    return {{}};
  }

};

}
