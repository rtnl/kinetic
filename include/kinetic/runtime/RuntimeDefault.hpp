#pragma once

#include "../_prelude.hpp"
#include "./Runtime.hpp"
#include "./Coroutine.hpp"
#include <chrono>

namespace KINETIC_NAMESPACE {

class RuntimeDefault : public Runtime {
private:
  std::atomic<bool> _flag_running;
  std::mutex _flag_running_m;
  std::condition_variable _flag_running_cv;

  std::deque<ArcCoroT> _q_coro_base;
  std::mutex _q_coro_base_m;
  std::condition_variable _q_coro_base_cv;

  std::deque<ArcCoroT> _q_coro_wait;
  std::mutex _q_coro_wait_m;
  std::condition_variable _q_coro_wait_cv;

  u64 _signal_next;
  std::mutex _signal_next_m;

  std::vector<std::thread> _worker_base_list;
  std::vector<std::thread> _worker_wait_list;

public:
  RuntimeDefault()
    : _flag_running(false)
    , _signal_next(1)
  {}

  ~RuntimeDefault() override {
    stop();
  }

  static u64 get_clock_nano() {
    return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
      ).count()
    );
  }

  void run() override {
    {
      std::lock_guard<std::mutex> lock(_flag_running_m);
      if (_flag_running) {
        return;
      }
      _flag_running = true;
    }

    const auto t = (std::max)(std::thread::hardware_concurrency(), 2u);
    for (u64 x = 0; x < t; x++) {
      _worker_base_list.emplace_back(&RuntimeDefault::run_worker_base, this, x);
    }

    _worker_wait_list.emplace_back(&RuntimeDefault::run_worker_wait, this);
  }

  void stop() override {
    {
      std::lock_guard<std::mutex> lock(_flag_running_m);
      if (!_flag_running) {
        return;
      }
      _flag_running = false;
    }

    _flag_running_cv.notify_all();

    _q_coro_base_cv.notify_all();
    _q_coro_wait_cv.notify_all();

    for (auto & worker : _worker_base_list) {
      if (worker.joinable()) {
        worker.join();
      }
    }
    for (auto & worker : _worker_wait_list) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  void wait() override {
    std::unique_lock<std::mutex> lock(_flag_running_m);
    _flag_running_cv.wait(lock, [this](){
      return !_flag_running;
    });
  }

  void schedule(const ArcCoroT & coro) override {
    {
      std::lock_guard<std::mutex> lock(_flag_running_m);
      if (!_flag_running) {
        return;
      }
    }

    {
      std::lock_guard<std::mutex> lock(_q_coro_base_m);
      _q_coro_base.push_back(coro);
      _q_coro_base_cv.notify_one();
    }
  }

  u64 create_signal() override {
    {
      std::lock_guard<std::mutex> lock(_flag_running_m);
      if (!_flag_running) {
        return 0;
      }
    }

    std::lock_guard<std::mutex> lock(_signal_next_m);

    const auto result = _signal_next;
    _signal_next += 1;

    return result;
  }

  void complete_signal(const u64 signal) override {

  }

private:
  void run_worker_base(const u64 thread_index) {
    while (true) {
      {
        std::lock_guard<std::mutex> lock(_flag_running_m);
        if (!_flag_running) {
          break;
        }
      }

      ArcCoroT coro = nullptr;

      {
        std::unique_lock<std::mutex> lock(_q_coro_base_m);
        _q_coro_base_cv.wait(lock, [this](){
          return !_flag_running || !_q_coro_base.empty();
        });

        if (!_q_coro_base.empty()) {
          coro = _q_coro_base.front();
          _q_coro_base.pop_front();
        }
      }

      if (coro == nullptr) {
        continue;
      }

      const auto ctx = CoroutineExecuteContext{};

      CoroutineExecuteResult res{};
      try {
        res = coro->execute(ctx);
      } catch (...) {
        continue;
      }

      switch (res) {
        case CoroutineExecuteResult::Complete: {
          break;
        }
        case CoroutineExecuteResult::Continue: {
          std::lock_guard<std::mutex> lock(_q_coro_base_m);
          _q_coro_base.push_back(coro);
          _q_coro_base_cv.notify_one();
          break;
        }
        case CoroutineExecuteResult::Wait: {
          std::lock_guard<std::mutex> lock(_q_coro_wait_m);
          _q_coro_wait.push_back(coro);
          _q_coro_wait_cv.notify_one();
          break;
        }
      }
    }
  }

  void run_worker_wait() {
    while (true) {
      {
        std::lock_guard<std::mutex> lock(_flag_running_m);
        if (!_flag_running) {
          break;
        }
      }

      std::vector<ArcCoroT> coro_list;

      {
        std::unique_lock<std::mutex> lock(_q_coro_wait_m);
        _q_coro_wait_cv.wait(lock, [this](){
          return !_flag_running || !_q_coro_wait.empty();
        });
        if (!_flag_running) {
          break;
        }

        while (!_q_coro_wait.empty()) {
          coro_list.push_back(_q_coro_wait.front());
          _q_coro_wait.pop_front();
        }
      }

      u64 wake = 0;

      for (const ArcCoroT & coro : coro_list) {
        if (coro->complete_condition(CoroutineExecuteConditionKind::Time, get_clock_nano())) {
          std::lock_guard<std::mutex> lock(_q_coro_base_m);
          _q_coro_base.push_back(coro);
          _q_coro_base_cv.notify_one();
        } else {
          std::lock_guard<std::mutex> lock(_q_coro_wait_m);
          _q_coro_wait.push_back(coro);
          _q_coro_wait_cv.notify_one();

          const auto coro_wake = coro->get_condition_value_min(CoroutineExecuteConditionKind::Time);
          if (coro_wake > 0 && (wake == 0 || coro_wake < wake)) {
            wake = coro_wake;
          }
        }
      }

      if (wake != 0) {
        const auto wake_t = std::chrono::steady_clock::time_point(std::chrono::nanoseconds(wake));

        std::unique_lock<std::mutex> lock(_q_coro_wait_m);
        _q_coro_wait_cv.wait_until(lock, wake_t, [this](){
          return !_flag_running || !_q_coro_wait.empty();
        });
      }
    }
  }
};

}
