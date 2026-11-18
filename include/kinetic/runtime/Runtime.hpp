#pragma once

#include "../_prelude.hpp"
#include "./Coroutine.hpp"

namespace KINETIC_NAMESPACE {

class Runtime {
public:
  using ArcCoroT = std::shared_ptr<Coroutine>;

  virtual ~Runtime() = default;

  virtual void run() = 0;

  virtual void stop() = 0;

  virtual void wait() = 0;

  virtual void schedule(const ArcCoroT & coro) = 0;

  virtual u64 create_signal() = 0;

  virtual void complete_signal(const u64 signal) = 0;
};

}
