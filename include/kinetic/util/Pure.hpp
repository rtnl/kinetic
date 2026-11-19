#pragma once

#include "../_prelude.hpp"

namespace KINETIC_NAMESPACE {

template <typename A>
static std::function<A (void)> pure(const A & value) {
  return [value]() -> A {
    return value;
  };
}

}
