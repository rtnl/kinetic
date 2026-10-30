#pragma once

#include "../_prelude.hpp"

namespace KINETIC_NAMESPACE {

class TestContext {
private:

public:
  template <typename A, typename B>
  void assert_eq(A && a, B && b) const {
    assert(a == b);
  }

  template <typename A, typename B>
  void assert_ne(A && a, B && b) const {
    assert(a != b);
  }
};

}
