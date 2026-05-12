#pragma once

#include "type.h"
#include "result.h"

namespace kinetic {

class Writer {
public:
  virtual ~Writer() = default;

  virtual Result<usize> write(const u8 * buf, usize size) = 0;

  Result<usize> write_all(const u8 * buf, usize size) {
    usize written = 0;

    while (written < size) {
      const auto write_r = write(&buf[written], size - written);
      if (write_r.is_err()) {
        return write_r;
      }

      const usize l = write_r.unwrap();
      if (l == 0) {
        return Result<usize>::err(ErrorKind::ValueNotAvailable, "zero write in write_all");
      }

      written += l;
    }

    return Result<usize>::ok(written);
  }
};

}
