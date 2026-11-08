#pragma once

#include "../_prelude.hpp"
#include "./Option.hpp"

namespace KINETIC_NAMESPACE {

class Linear {
public:
  template <typename T>
  static auto eq(
    const T & a, const usize a_size, const usize a_offset,
    const T & b, const usize b_size, const usize b_offset,
    const usize length
  ) -> bool {
    if (a_offset > a_size || b_offset > b_size) {
      return false;
    }

    if (length > (a_size - a_offset) || length > (b_size - b_offset)) {
      return false;
    }

    for (usize x = 0; x < length; ++x) {
      if (a[a_offset + x] != b[b_offset + x]) {
        return false;
      }
    }

    return true;
  }

  template <typename T>
  static auto find(const T & a, const usize a_size, const T & b, const usize b_size) -> Option<usize> {
    if (b_size > a_size) {
      return {};
    }

    if (b_size == 0) {
      return {0};
    }

    const auto t = a_size - b_size;
    usize match = 0;
    for (usize x = 0; x <= t; x++) {
      if (eq<T>(a, a_size, x, b, b_size, 0, b_size)) {
        return {x};
      }
    }

    return {};
  }

  template <typename T>
  static auto has_prefix(const T & a, const usize a_size, const T & b, const usize b_size) -> bool {
    return eq<T>(a, a_size, 0, b, b_size, 0, b_size);
  }

  template <typename T>
  static auto has_suffix(const T & a, const usize a_size, const T & b, const usize b_size) -> bool {
    return eq<T>(a, a_size, a_size - b_size, b, b_size, 0, b_size);
  }
};

}
