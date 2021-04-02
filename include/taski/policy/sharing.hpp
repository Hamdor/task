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
#include <thread>
#include <memory>
#include <condition_variable>

#include "taski/detail/work_item.hpp"
#include "taski/detail/atomic_queue.hpp"

namespace taski {

/// Work sharing policy for scheduler.
template <size_t Workers>
class sharing {
protected:
  sharing() = default;

  /// Worker implementation of work sharing policy.
  struct worker {
    worker() = default;

    /// Worker main loop.
    void run(sharing* ctx) {
      while(running_) {
        auto ptr = ctx->internal_dequeue();
        if (ptr == nullptr) {
          std::unique_lock guard{ctx->lock_};
          ctx->cv_.wait(guard);
          continue;
        }
        (*ptr)();
      }
    }

    /// Called once at initialization of scheduler.
    void init(sharing* ctx) {
      running_ = true;
      ctx_ = ctx;
      thread_ = std::thread{[&] { this->run(ctx_); }};
    }

    /// Called just before scheduler is destroyed (e.g. goes out of scope).
    inline void set_running(bool value) { running_ = value; }

    /// Called just before scheduler is destroyed (e.g. goes out of scope).
    void join() {
      running_ = false;
      ctx_->cv_.notify_all();
      thread_.join();
    }

    std::thread thread_;     /// Thread of worker
    volatile bool running_;  /// Indicates if worker should still run
    sharing* ctx_;           /// Context (parent policy)
  };

  friend struct worker;

  /// Called by the scheduler on creation.
  void init() {
    for (auto& w : workers_)
      w.init(this);
  }

  /// Called by the scheduler when a task is enqueued to the scheduler.
  template <class T, class... Ts>
  auto internal_enqueue(T&& t, Ts&&... ts) {
    using work_item_t = detail::work_item<T, Ts...>;
    auto ptr = std::make_unique<work_item_t>(std::forward<T>(t),
                                             std::forward<Ts>(ts)...);
    auto future = ptr->future();
    queue_.append(std::move(ptr));
    cv_.notify_all();
    return future;
  }

  /// Called by a worker to request a new job.
  auto internal_dequeue() {
    return queue_.take_head();
  }

  /// Called when the scheduler is about to get destroyed
  /// (e.g. leave its scope).
  void shutdown() {
    while(!queue_.empty())
      continue; // TODO: Use a cv...
    // First, set running in all workers to false
    for (auto& w : workers_)
      w.set_running(false);
    // Now join all threads...
    for (auto& w : workers_)
      w.join();
  }

private:
  std::array<worker, Workers> workers_;            /// Worker threads
  detail::atomic_queue<detail::storable> queue_;   /// Shared work queue
  std::condition_variable cv_;                     /// CV for item queue
  std::mutex lock_;                                /// Mutex for CV
};

} // namespace taski
