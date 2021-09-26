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

#include "taski/detail/dynamic.hpp"
#include "taski/detail/work_item.hpp"

#include <queue>
#include <thread>
#include <memory>
#include <vector>
#include <condition_variable>

namespace taski {

/// Work sharing policy for scheduler.
template <int Workers>
class sharing {
protected:
  sharing() {
    auto nthreads = detail::number_of_threads<Workers>();
    workers_.reserve(nthreads);
    for (size_t i = 0; i < nthreads; ++i) {
      workers_.push_back(std::make_unique<worker>());
    }
    for (auto& w : workers_) {
      w->init(this);
    }
  }

  sharing(const sharing&) = delete;
  sharing(sharing&&) = delete;

  sharing& operator=(const sharing&) = delete;
  sharing& operator=(sharing&&) = delete;

  virtual ~sharing() {
    set_running(false);
    // Now join all threads...
    for (auto& w : workers_)
      w->join();
  }

  void set_running(bool running) {
    std::unique_lock guard{lock_};
    running_ = running;
    cv_.notify_all();
  }

  /// Worker implementation of work sharing policy.
  struct worker {
    worker() = default;

    /// Worker main loop.
    void run() {
      auto pred = [this] {
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
  decltype(auto) internal_enqueue(T&& t, Ts&&... ts) {
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
  std::atomic<bool> running_ = true;
  std::vector<std::unique_ptr<worker>> workers_;         /// Worker threads
  std::queue<std::unique_ptr<detail::storable>> queue_;  /// Shared work queue
  std::condition_variable cv_;                           /// CV for item queue
  std::mutex lock_;                                      /// Mutex for CV
};

} // namespace taski
