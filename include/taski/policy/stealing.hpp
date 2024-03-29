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
#include "taski/detail/index_table.hpp"
#include "taski/detail/atomic_queue.hpp"
#include "taski/detail/xorwow_engine.hpp"

#include <random>
#include <memory>
#include <utility>

namespace taski {


/// Work stealing policy for scheduler.
template <int Workers>
class stealing {
  static_assert(Workers == detail::dynamic || Workers >= 2,
                "Who to steal from? You need at least two workers.");
protected:
  stealing() {
    auto nthreads = detail::number_of_threads<Workers>();
    workers_.reserve(nthreads);
    for (size_t i = 0; i < nthreads; ++i) {
      workers_.push_back(std::make_unique<worker>(i, this));
    }
    for (size_t i = 0; i < nthreads; ++i) {
      workers_[i]->start();
    }
  }

  stealing(const stealing&) = delete;
  stealing(stealing&&) = delete;

  stealing& operator=(const stealing&) = delete;
  stealing& operator=(stealing&&) = delete;

  virtual ~stealing() {
    for (auto& w : workers_)
      w->set_running(false);
    for (auto& w : workers_)
      w->join();
  }

  /// Worker implementation of work sharing policy.
  struct worker {
    worker(size_t idx, stealing* ctx) {
      idx_ = idx;
      running_ = true;
      ctx_ = ctx;
    }

    /// Worker main loop.
    void run() {
      // We use a index table which does not include our
      // own index. This allows to only sample valid indices
      // to steal from.
      auto idx_table = detail::generate_index_table(ctx_->workers_.size() - 1, idx_);
      std::uniform_int_distribution<size_t> dist(0, idx_table.size() - 1);
      while(running_ || !queue_.empty()) {
        auto ptr = queue_.take_head();
        if (ptr == nullptr) {
          size_t idx = idx_table[dist(generator_)];
          ptr = ctx_->workers_[idx]->queue_.take_head();
          if (!ptr) {
            std::this_thread::yield();
            continue;
          }
        }
        (*ptr)();
      }
    }

    /// Called once at initialization of scheduler.
    void start() {
      thread_ = std::thread{[this] { this->run(); }};
    }

    /// Called just before scheduler is destroyed (e.g. goes out of scope).
    inline void set_running(bool value) { running_ = value; }

    /// Enqueue to workers queue.
    template <class T>
    void enqueue(T&& t) {
      queue_.append(std::forward<T>(t));
    }

    /// Called just before scheduler is destroyed (e.g. goes out of scope).
    void join() {
      running_ = false;
      ctx_->cv_.notify_all();
      thread_.join();
    }

    std::thread thread_;         /// Thread of worker
    std::atomic<bool> running_;  /// Indicates if worker should still run
    stealing* ctx_;              /// Context (parent policy)
    size_t idx_;                 /// Id of worker
    detail::atomic_queue<detail::storable> queue_;  /// Job queue
    detail::xorwow generator_;                      /// Random generator for stealing
  };

  friend struct worker;

  template <class T, class... Ts>
  decltype(auto) internal_enqueue(T&& t, Ts&&... ts) {
    using work_item_t = detail::work_item<T, Ts...>;
    auto ptr = std::make_unique<work_item_t>(std::forward<T>(t),
                                             std::forward<Ts>(ts)...);
    auto future = ptr->future();
    auto idx = (last_enqueued_ + 1) % workers_.size();
    workers_[idx]->enqueue(std::move(ptr));
    last_enqueued_ = idx;
    return future;
  }

private:
  std::vector<std::unique_ptr<worker>> workers_;  /// Worker threads
  size_t last_enqueued_ = 0;      /// Idx of worker which got the last task.
  std::condition_variable cv_;
  std::mutex lock_;
};

} // namespace taski
