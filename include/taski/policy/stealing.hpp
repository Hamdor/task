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

#ifndef TASKI_POLICY_STEALING_HPP
#define TASKI_POLICY_STEALING_HPP

#include <random>
#include <utility>

namespace taski {

/// Work stealing policy for scheduler.
template <size_t Workers>
class stealing {
  static_assert(Workers >= 2,
                "Who to steal from? You need at least two workers.");
protected:
  stealing() : last_enqueued_{0} {
    // nop
  }

  /// Worker implementation of work sharing policy.
  struct worker {
    worker() = default;

    /// Worker main loop.
    void run(stealing* ctx) {
      while(running_ || !queue_.empty()) {
        auto ptr = queue_.take_head();
        if (ptr == nullptr) {
          std::uniform_int_distribution<size_t> dist(0, Workers);
          // Try to steal from another worker
          size_t idx = 0;
          do {
            idx = dist(generator_);
          } while(this != &ctx->workers_[idx]);
          ptr = ctx->workers_[idx].queue_.take_head();
          if (!ptr) {
            std::this_thread::yield();
            continue;
          }
        }
        (*ptr)();
        delete ptr;
      }
    }

    /// Called once at initialization of scheduler.
    void init(size_t idx, stealing* ctx) {
      idx_ = idx;
      running_ = true;
      ctx_ = ctx;
      thread_ = std::thread{[&] { this->run(ctx_); }};
    }

    /// Called just before scheduler is destroyed (e.g. goes out of scope).
    inline void set_running(bool value) { running_ = value; }

    ///
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

    std::thread thread_;      /// Thread of worker
    volatile bool running_;   /// Indicates if worker should still run
    stealing* ctx_;           /// Context (parent policy)
    size_t idx_;              /// Id of worker
    detail::atomic_queue<detail::storable> queue_;  /// Job queue
    std::mt19937 generator_;  /// Random generator for stealing
  };

  friend struct worker;

  void init() {
    for (size_t i = 0; i < workers_.size(); ++i)
      workers_[i].init(i, this);
  }

  template <class T, class... Ts>
  auto internal_enqueue(T&& t, Ts&&... ts) {
    auto ptr = new detail::work_item<T, Ts...>(std::forward<T>(t),
                                               std::forward<Ts>(ts)...);
    workers_[last_enqueued_].enqueue(ptr);
    last_enqueued_ = (last_enqueued_ + 1) % Workers;
    return ptr->future();
  }

  void internal_dequeue() {
    // nop
  }

  void shutdown() {
    for (auto& w : workers_)
      w.set_running(false);
    for (auto& w : workers_)
      w.join();
  }

private:
  std::array<worker, Workers> workers_;    /// Worker threads
  size_t last_enqueued_;                   /// Idx of worker which got the
                                           /// last task.
  std::condition_variable cv_;
  std::mutex lock_;
};

} // namespace taski

#endif // TASKI_POLICY_STEALING_HPP
