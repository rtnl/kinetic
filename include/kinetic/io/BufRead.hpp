#pragma once

#include "../_prelude.hpp"
#include "./Read.hpp"

namespace KINETIC_NAMESPACE {

class BufRead : public Read {
public:
  ~BufRead() override = default;

  virtual Result<usize> fill_buf() = 0;

  virtual void consume(const usize size) = 0;

  virtual Result<usize> read_until(const u8 * bytes, const usize size, std::vector<u8> & out) = 0;

  virtual Result<usize> skip_until(const u8 * bytes, const usize size) = 0;
};

}
