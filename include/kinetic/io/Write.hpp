#pragma once

#include "../_prelude.hpp"
#include "./Config.hpp"
#include "../util/Result.hpp"
#include "../util/Unit.hpp"

namespace KINETIC_NAMESPACE {

class Write {
  static_assert(KINETIC_BUFFER_SIZE > 0, "KINETIC_BUFFER_SIZE positive");

public:
  virtual ~Write() = default;

  virtual Result<usize> write(const u8 * from, const usize limit) = 0;

  Result<Unit> write_all(const u8 * from, const usize size) {
    usize s_from = 0;
    usize s_left = size;

    if (from == nullptr) {
      return {ErrorKind::ValueNone, "from is null"};
    }
    if (size == 0) {
      return {{}};
    }

    while (s_left > 0) {
      const Result<usize> io_r = write(
        &from[s_from],
        (std::min)(s_left, KINETIC_BUFFER_SIZE));
      if (io_r.is_err()) {
        return {io_r.get_error()};
      }

      const usize io_n = io_r.get();
      if (io_n == 0) {
        return {ErrorKind::IoStalled, "zero bytes written"};
      }
      if (io_n > s_left) {
        return {ErrorKind::ValueIncorrect, "wrote more bytes than limit"};
      }

      s_from += io_n;
      s_left -= io_n;
    }

    return {{}};
  }

};

}
