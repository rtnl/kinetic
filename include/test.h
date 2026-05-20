#pragma once

#include <./type.h>
#include <./program.h>
#include <stdexcept>

namespace kinetic {

class TestContext {

};

class TestProgram : kinetic::Program<u8> {
private:


public:
  int run() override {
    throw std::runtime_error("unimplemented");
  }
};

}

#define __KINETIC_TEST_CASE_IMPL_FUNC(_NAME) void _NAME(const kinetic::TestContext & ctx)

#define __KINETIC_TEST_CASE_IMPL_CLASS(_NAME, _FUNC) class _NAME { _NAME() { _FUNC(kinetic::TestContext()); } }; static _NAME _NAME_##_I();

#define __KINETIC_TEST_CASE_IMPL(_KEY, _NAME)        \
__KINETIC_TEST_CASE_IMPL_FUNC(_KEY_##_F);            \
__KINETIC_TEST_CASE_IMPL_CLASS(_KEY_##_C, _KEY_##_F) \
__KINETIC_TEST_CASE_IMPL_FUNC(_KEY_##_F)

#define KINETIC_TEST_CASE(_NAME) __KINETIC_TEST_CASE_IMPL(__KINETIC_TEST_CASE_##__COUNTER__, _NAME)
