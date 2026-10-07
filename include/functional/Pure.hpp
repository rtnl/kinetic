#pragma once

#include "../prelude.hpp"

namespace KINETIC_NAMESPACE {

template <typename A>
auto pure(A value) -> std::function<A (void)> {
  return std::function<A (void)>([value](void) -> A {
    return value;
  });
}

}
