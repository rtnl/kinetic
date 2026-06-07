#pragma once

#include <./meta.h>
#include <./type.h>
#include <./log.h>
#include <./program.h>
#include <stdexcept>

namespace kinetic {

enum class TestContextState {
  Fail,
  Pass,
};

inline std::string Display(const TestContextState state) {
  switch (state) {
    case TestContextState::Fail: return "fail";
    case TestContextState::Pass: return "pass";
    default:
      throw std::runtime_error("unimplemented");
  }
}

class TestContext {
private:
  std::string _name;

  TestContextState _state;

public:
  TestContext(const std::string & name)
    : _name(name)
    , _state(TestContextState::Fail)
  {}

  ~TestContext() {
    _LOG_DEFAULT.Test(_name + " - " + Display(_state));

    if (get_state() != TestContextState::Pass) {
      ::exit(1);
    }
  }

  std::string get_name() const {
    return _name;
  }

  KINETIC_GETTER(_state, state)

  void ok() {
    _state = TestContextState::Pass;
  }
};

class TestProgram : kinetic::Program<u8> {
private:


public:
  int run() override {
    return 0;
  }
};

}

#define __KINETIC_TEST_CASE_IMPL_FUNC(_NAME) void _NAME(kinetic::TestContext & test_ctx)

#ifdef KINETIC_TEST
#define __KINETIC_TEST_CASE_IMPL_INSTANCE(_NAME) static _NAME _NAME_##_I;
#else
#define __KINETIC_TEST_CASE_IMPL_INSTANCE(_NAME) ;
#endif

#define __KINETIC_TEST_CASE_IMPL_CLASS(_NAME, _KEY_C, _FUNC) \
class _KEY_C { public: _KEY_C() { kinetic::TestContext test_ctx(#_NAME); _FUNC(test_ctx); } }; \
__KINETIC_TEST_CASE_IMPL_INSTANCE(_KEY_C)

#define __KINETIC_TEST_CASE_IMPL(_KEY, _NAME)        \
__KINETIC_TEST_CASE_IMPL_FUNC(_KEY##_F);            \
__KINETIC_TEST_CASE_IMPL_CLASS(_NAME, _KEY##_C, _KEY##_F) \
__KINETIC_TEST_CASE_IMPL_FUNC(_KEY##_F)

#ifdef KINETIC_TEST
#define KINETIC_TEST_CASE(_NAME) __KINETIC_TEST_CASE_IMPL(__KINETIC_TEST_CASE_##_NAME, _NAME)
#else
#define KINETIC_TEST_CASE(_NAME) template<typename T = void> void kinetic_test_skip_##_NAME(kinetic::TestContext & test_ctx)
#endif
