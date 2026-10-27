#pragma once

#include "../_prelude.hpp"

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
