#pragma once

#include <./type.h>
#include <./program.h>

namespace kinetic {

class TestContext {

};

class TestProgram : kinetic::Program<u8> {
private:


public:
  int run() override {
    return 0;
  }
};

}

#define __KINETIC_TEST_CASE_IMPL_FUNC(_NAME) void _NAME(const kinetic::TestContext & ctx)

#ifdef KINETIC_TEST
#define __KINETIC_TEST_CASE_IMPL_INSTANCE(_NAME) static _NAME _NAME_##_I;
#else
#define __KINETIC_TEST_CASE_IMPL_INSTANCE(_NAME) ;
#endif

#define __KINETIC_TEST_CASE_IMPL_CLASS(_NAME, _FUNC) \
class _NAME { public: _NAME() { _FUNC(kinetic::TestContext()); } }; \
__KINETIC_TEST_CASE_IMPL_INSTANCE(_NAME)

#define __KINETIC_TEST_CASE_IMPL(_KEY, _NAME)        \
__KINETIC_TEST_CASE_IMPL_FUNC(_KEY##_F);            \
__KINETIC_TEST_CASE_IMPL_CLASS(_KEY##_C, _KEY##_F) \
__KINETIC_TEST_CASE_IMPL_FUNC(_KEY##_F)

#ifdef KINETIC_TEST
#define KINETIC_TEST_CASE(_NAME) __KINETIC_TEST_CASE_IMPL(__KINETIC_TEST_CASE_##counter, _NAME)
#else
#define KINETIC_TEST_CASE(_NAME) template<typename T = void> void kinetic_test_skip_##__COUNTER__()
#endif
