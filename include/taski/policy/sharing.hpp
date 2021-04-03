/*******************************************************************************
 *                            _            _    _                              *
 *                           | |          | |  (_)                             *
 *                           | |_ __ _ ___| | ___                              *
 *                           | __/ _` / __| |/ / |                             *
 *                           | || (_| \__ \   <| |                             *
 *                            \__\__,_|___/_|\_\_|                             *
 *                                                                             *
 * Copyright (C) 2015 - 2021                                                   *
 * Marian Triebe                                                               *
 *                                                                             *
 * Distributed under the terms and conditions of the BSD 3-Clause License      *
 * See accompanying file LICENSE.                                              *
 *                                                                             *
 * If you did not receive a copy of the license file, see                      *
 * http://opensource.org/licenses/BSD-3-Clause                                 *
 *******************************************************************************/

#pragma once

#include <array>
#include <queue>
#include <thread>
#include <memory>
#include <condition_variable>

#include "taski/detail/work_item.hpp"

namespace taski {

/// Work sharing policy for scheduler.
template <size_t Workers>
class sharing {
protected:
  sharing() : running_{true} {
    for (auto& w : workers_)
      w.init(this);
  }

  virtual ~sharing() {
    {
      std::unique_lock guard{lock_};
      running_ = false;
      cv_.notify_all();
    }
    // Now join all threads...
    for (auto& w : workers_)
      w.join();
  }

  /// Worker implementation of work sharing policy.
  struct worker {
    worker() = default;

    /// Worker main loop.
    void run() {
      auto pred = [&]{
        std::unique_lock guard{ctx_->lock_};
        return !ctx_->queue_.empty() || ctx_->running_;
      };
      while(pred()) {
        if (auto ptr = ctx_->internal_dequeue())
          (*ptr)();
      }
    }

    /// Called once at initialization of scheduler.
    void init(sharing* ctx) {
      ctx_ = ctx;
      thread_ = std::thread{[this] { this->run(); }};
    }

    /// Called just before scheduler is destroyed (e.g. goes out of scope).
    void join() {
      thread_.join();
    }

    std::thread thread_;     /// Thread of worker
    sharing* ctx_;           /// Context (parent policy)
  };

  friend struct worker;

  /// Called by the scheduler when a task is enqueued to the scheduler.
  template <class T, class... Ts>
  auto internal_enqueue(T&& t, Ts&&... ts) {
    using work_item_t = detail::work_item<T, Ts...>;
    auto ptr = std::make_unique<work_item_t>(std::forward<T>(t),
                                             std::forward<Ts>(ts)...);
    auto future = ptr->future();
    std::unique_lock guard{lock_};
    queue_.push(std::move(ptr));
    cv_.notify_all();
    return future;
  }

  /// Called by a worker to request a new job.
  auto internal_dequeue() {
    std::unique_ptr<detail::storable> ptr;
    auto pred = [this] { return !queue_.empty() || !running_; };
    std::unique_lock guard{lock_};
    cv_.wait(guard, pred);
    if (!queue_.empty()) {
      ptr = std::move(queue_.front());
      queue_.pop();
    }
    return ptr;
  }

private:
  volatile bool running_;
  std::array<worker, Workers> workers_;                  /// Worker threads
  std::queue<std::unique_ptr<detail::storable>> queue_;  /// Shared work queue
  std::condition_variable cv_;                           /// CV for item queue
  std::mutex lock_;                                      /// Mutex for CV
};

} // namespace taski
