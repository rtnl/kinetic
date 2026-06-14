#pragma once

#include <stdexcept>

#include "meta.h"

namespace kinetic {

enum class EitherKind {
  Left,
  Right,
};

template <typename A, typename B>
class Either {
private:
  EitherKind _kind;
  A _l;
  B _r;

public:
  explicit Either(const A & l) noexcept
    : _kind(EitherKind::Left)
    , _l(l)
    , _r(B())
  {}

  explicit Either(const B & r) noexcept
    : _kind(EitherKind::Right)
    , _l(A())
    , _r(r)
  {}

  KINETIC_GETTER(_kind, kind)

  bool get_is_kind(const EitherKind & kind) const {
    return get_kind() == kind;
  }

  bool get_is_left() const {
    return get_is_kind(EitherKind::Left);
  }

  bool get_is_right() const {
    return get_is_kind(EitherKind::Right);
  }

  const A & get_l() const {
    switch (get_kind()) {
      case EitherKind::Left:
        return _l;
      default:
        throw std::logic_error("Either is not left");
    }
  }

  const B & get_r() const {
    switch (get_kind()) {
      case EitherKind::Right:
        return _r;
      default:
        throw std::logic_error("Either is not right");
    }
  }
};

}
