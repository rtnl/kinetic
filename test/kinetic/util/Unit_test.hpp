#pragma once

#include <kinetic.hpp>

KineticTestCase(KineticUtilUnit_Eq) {
  kinetic::Unit a;
  ctx.assert_eq(a, a);
  kinetic::Unit b;
  ctx.assert_eq(a, b);
}

KineticTestCase(KineticUtilUnit_NotEq) {
  kinetic::Unit a;
  ctx.assert_ne((a != a), true);
  kinetic::Unit b;
  ctx.assert_ne((a != b), true);
}
