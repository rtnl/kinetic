#pragma once

#include "../_prelude.hpp"

namespace KINETIC_NAMESPACE {

enum class EitherKind {
  L,
  R,
};

template <typename L, typename R>
class Either {
private:
  union Inner {
    L l;
    R r;

    Inner() {}
    ~Inner() {}
  } _inner;

  EitherKind _kind;
public:
  explicit Either(const L & value) {
    new (&_inner.l) L(value);
    _kind = EitherKind::L;
  }

  explicit Either(const R & value) {
    new (&_inner.r) R(value);
    _kind = EitherKind::R;
  }

  // vibe coded
  Either(const Either & other) {
    if (other.is_l()) {
      new (&_inner.l) L(other._inner.l);
      _kind = EitherKind::L;
    } else {
      new (&_inner.r) R(other._inner.r);
      _kind = EitherKind::R;
    }
  }

  // vibe coded
  Either & operator=(const Either & other) {
    if (this == &other) {
      return *this;
    }

    if (is_l() && other.is_l()) {
      _inner.l = other._inner.l;
    } else if (is_r() && other.is_r()) {
      _inner.r = other._inner.r;
    } else if (other.is_l()) {
      _inner.r.~R();
      new (&_inner.l) L(other._inner.l);
      _kind = EitherKind::L;
    } else {
      _inner.l.~L();
      new (&_inner.r) R(other._inner.r);
      _kind = EitherKind::R;
    }

    return *this;
  }

  ~Either() {
    switch (_kind) {
      case EitherKind::L: {
        _inner.l.~L();
        break;
      }
      case EitherKind::R: {
        _inner.r.~R();
        break;
      }
    }
  }

  EitherKind get_kind() const {
    return _kind;
  }

  bool is_l() const {
    return _kind == EitherKind::L;
  }

  bool is_r() const {
    return _kind == EitherKind::R;
  }

  const L & get_l() const {
    assert(_kind == EitherKind::L);
    return _inner.l;
  }

  const R & get_r() const {
    assert(_kind == EitherKind::R);
    return _inner.r;
  }
};

}
