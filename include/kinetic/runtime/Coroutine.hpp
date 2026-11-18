#pragma once

#include "../_prelude.hpp"

namespace KINETIC_NAMESPACE {

enum class CoroutineWaitMode {
  Any,
  All,
};

enum class CoroutineExecuteContext {

};

enum class CoroutineExecuteResult {
  Complete,
  Continue,
  Wait,
};

enum class CoroutineExecuteConditionKind {
  Time,
  Signal,
};

class CoroutineExecuteCondition {
private:
  u64 _key;

  CoroutineExecuteConditionKind _kind;

  u64 _value;

public:
  CoroutineExecuteCondition(
    const u64                           key,
    const CoroutineExecuteConditionKind kind,
    const u64                           value
  )
    : _key(key)
    , _kind(kind)
    , _value(value)
  {}

  u64 get_key() const {
    return _key;
  }

  CoroutineExecuteConditionKind get_kind() const {
    return _kind;
  }

  u64 get_value() const {
    return _value;
  }
};

class Coroutine {
public:
  virtual ~Coroutine() = default;

  virtual void set_wait_mode(const CoroutineWaitMode mode) = 0;

  virtual CoroutineWaitMode get_wait_mode() = 0;

  virtual void add_condition(const CoroutineExecuteCondition & cond) = 0;

  virtual bool complete_condition(const CoroutineExecuteConditionKind kind, const u64 value) = 0;

  virtual u64 get_condition_value_min(const CoroutineExecuteConditionKind kind) = 0;

  virtual u64 get_condition_value_max(const CoroutineExecuteConditionKind kind) = 0;

  virtual CoroutineExecuteResult execute(const CoroutineExecuteContext & ctx) = 0;
};

}
