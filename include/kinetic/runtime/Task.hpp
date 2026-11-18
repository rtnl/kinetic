#pragma once

#include "../_prelude.hpp"
#include "./Coroutine.hpp"

namespace KINETIC_NAMESPACE {

class Task : public Coroutine {
public:
  using HandlerT = std::function<CoroutineExecuteResult (Task & self, const CoroutineExecuteContext & ctx)>;

protected:
  std::mutex _mutex;

  CoroutineWaitMode _wait_mode;

  std::vector<CoroutineExecuteCondition> _condition_list;

  u64 _handler_next;

  std::unordered_map<u64, HandlerT> _handler_list;

public:
  Task()
    : _handler_next(0)
    , _handler_list()
  {}

  ~Task() override = default;

  void set_wait_mode(const CoroutineWaitMode mode) override {
    std::lock_guard<std::mutex> lock(_mutex);

    _wait_mode = mode;
  }

  CoroutineWaitMode get_wait_mode() override {
    std::lock_guard<std::mutex> lock(_mutex);

    return _wait_mode;
  }

  void add_condition(const CoroutineExecuteCondition & cond) override {
    std::lock_guard<std::mutex> lock(_mutex);

    _condition_list.push_back(cond);
  }

  bool complete_condition(const CoroutineExecuteConditionKind kind, const u64 value) override {
    std::lock_guard<std::mutex> lock(_mutex);

    std::vector<u64> match;

    for (const auto & cond : _condition_list) {
      if (kind == CoroutineExecuteConditionKind::Signal) {
        if (cond.get_value() == value) {
          match.push_back(cond.get_key());
        }
      } else {
        if (cond.get_value() < value) {
          match.push_back(cond.get_key());
        }
      }
    }

    std::vector<CoroutineExecuteCondition> condition_list_new;
    for (const auto & cond : _condition_list) {
      bool flag_keep = true;

      for (const auto key : match) {
        if (cond.get_key() == key) {
          flag_keep = false;
          break;
        }
      }

      if (flag_keep) {
        condition_list_new.push_back(cond);
      }
    }
    _condition_list = condition_list_new;

    switch (_wait_mode) {
      case CoroutineWaitMode::Any: {
        return !match.empty() || _condition_list.empty();
      }
      case CoroutineWaitMode::All: {
        return _condition_list.empty();
      }
      default:
        return false;
    }
  }

  u64 get_condition_value_min(const CoroutineExecuteConditionKind kind) override {
    std::lock_guard<std::mutex> lock(_mutex);

    u64 value = 0;

    for (const auto & cond : _condition_list) {
      if (cond.get_kind() != kind) {
        continue;
      }

      const auto cond_value = cond.get_value();
      if (value == 0 || cond_value < value) {
        value = cond_value;
      }
    }

    return value;
  }

  u64 get_condition_value_max(const CoroutineExecuteConditionKind kind) override {
    std::lock_guard<std::mutex> lock(_mutex);

    u64 value = 0;

    for (const auto & cond : _condition_list) {
      if (cond.get_kind() != kind) {
        continue;
      }

      const auto cond_value = cond.get_value();
      if (value == 0 || cond_value > value) {
        value = cond_value;
      }
    }

    return value;
  }

  CoroutineExecuteResult execute(const CoroutineExecuteContext & ctx) override {
    std::lock_guard<std::mutex> lock(_mutex);

    return _handler_list[_handler_next](*this, ctx);
  }

  void register_handler(const u64 key, const HandlerT & handler) {
    std::lock_guard<std::mutex> lock(_mutex);

    _handler_list[key] = handler;
  }
};

}
