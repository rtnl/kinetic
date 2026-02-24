#pragma once

namespace kinetic {

class Unit {
public:
  bool operator==(const Unit &_) const noexcept { return true; }

  bool operator!=(const Unit &_) const noexcept { return false; }
};

} // namespace kinetic
