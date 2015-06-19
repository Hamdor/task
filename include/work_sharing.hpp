/*******************************************************************************
 *                            _            _    _                              *
 *                           | |          | |  (_)                             *
 *                           | |_ __ _ ___| | ___                              *
 *                           | __/ _` / __| |/ / |                             *
 *                           | || (_| \__ \   <| |                             *
 *                            \__\__,_|___/_|\_\_|                             *
 *                                                                             *
 * Copyright (C) 2015                                                          *
 * Marian Triebe  <marian.triebe  (at) haw-hamburg.de>                         *
 *                                                                             *
 * Distributed under the terms and conditions of the BSD 3-Clause License      *
 * See accompanying file LICENSE.                                              *
 *                                                                             *
 * If you did not receive a copy of the license file, see                      *
 * http://opensource.org/licenses/BSD-3-Clause                                 *
 *******************************************************************************/

#ifndef WORK_SHARING_HPP
#define WORK_SHARING_HPP

#include <future>
#include <thread>

#include "work_item.hpp"
#include "atomic_queue.hpp"
#include "scheduler_data.hpp"

namespace {

using queue_t = atomic_queue<storeable>;
constexpr static size_t m_number_of_workers = CONCURRENCY_LEVEL;

} // namespace <anonymous>

class work_sharing {
  struct worker {
    worker() {
      alive = true;
      m_thread = std::thread([&]() {
        while(work_sharing::instance == nullptr) {
          // wait till instance is initialized...
        }
        auto& ws = *work_sharing::instance;
        storeable* fun = nullptr;
        while(alive) {
          if (ws.m_jobs.empty()) {
            ws.m_data.m_empty.notify_all();
            std::unique_lock<std::mutex> lock(ws.m_data.m_lock);
            ws.m_data.m_new.wait(lock);
            if (!alive) return;
          } else {
            fun = ws.m_jobs.take_head();
          }
          if (fun) fun->exec();
          delete fun;
        }
      });
    }
    std::thread m_thread;
    volatile bool alive;
  };

  friend worker;

  work_sharing() = default;

  template<typename T, typename... Xs>
  auto emplace(work_item<T, Xs...>* ptr) {
    auto future = ptr->get_future();
    m_jobs.append(ptr);
    m_data.m_new.notify_all();
    return future;
  }

 public:
  template<typename T, typename... Xs>
  auto run(T&& t, Xs&&... xs) {
    return emplace(new work_item<T, Xs...>(std::move(t),
                                           std::forward<Xs>(xs)...));
  }

  template<typename T, typename... Xs>
  auto run(T& t, Xs&... xs) {
    return emplace(new work_item<T, Xs...>(t, xs...));
  }

  static work_sharing& get_instance();
  void shutdown();

 private:
  static std::mutex s_mtx;
  static work_sharing* instance;

  scheduler_data m_data;
  queue_t        m_jobs;

  static worker m_workers[m_number_of_workers];
};

#endif // WORK_SHARING_HPP
