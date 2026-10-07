#pragma once

#include "../prelude.hpp"

namespace KINETIC_NAMESPACE {

class Unit {
public:
  bool operator==(const Unit & _) const noexcept {
    return true;
  }

  bool operator!=(const Unit & _) const noexcept {
    return false;
  }
};

}
