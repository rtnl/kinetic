#pragma once

#include "../_prelude.hpp"
#include "./TestContext.hpp"

#define __KineticTestCase(_KEY, _CLASS, _INSTANCE)              \
class _CLASS {                                                  \
  using TestContext = KINETIC_NAMESPACE::TestContext;           \
  public:                                                       \
  _CLASS(const std::string & key) {                             \
    run_wrapper(key);                                           \
  }                                                             \
  static void run(const TestContext & ctx);                     \
  static void run_wrapper(const std::string & key) {            \
    const TestContext ctx;                                      \
    std::cerr << "[test] run (" << key << ")" << std::endl;     \
    run(ctx);                                                   \
    std::cerr << "[test] pass (" << key << ")" << std::endl;    \
  }                                                             \
};                                                              \
static _CLASS _INSTANCE{KINETIC_STR(_KEY)};                     \
inline void _CLASS::run(const TestContext & ctx)

#if defined(KINETIC_TEST)
#define KineticTestCase(_KEY) __KineticTestCase(_KEY, KINETIC_CONCAT(__KineticTestCase_C, _KEY), KINETIC_CONCAT(__KineticTestCase_F, __COUNTER__))
#else
#define KineticTestCase(_KEY) static void KINETIC_CONCAT(__KineticTestCase_F, __COUNTER__)(const KINETIC_NAMESPACE::TestContext & ctx)
#endif
