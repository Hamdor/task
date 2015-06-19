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

#ifndef WORK_STEALING_HPP
#define WORK_STEALING_HPP

#include <atomic>
#include <future>
#include <random>
#include <thread>

#include "work_item.hpp"
#include "atomic_queue.hpp"
#include "scheduler_data.hpp"

namespace {

using queue_t = atomic_queue<storeable>;
constexpr static size_t m_number_of_workers = CONCURRENCY_LEVEL;

} // namespace <anonymous>

class work_stealing {
  struct worker {
    worker() : m_rdevice(), m_rengine(m_rdevice()) {
      alive = true;
      m_thread = std::thread([&]() {
        while(work_stealing::instance == nullptr) {
          // wait till instance is initialized...
        }
        auto& ws = *work_stealing::instance;
        storeable* fun = nullptr;
        auto do_steal = [&]() {
          if (m_number_of_workers >= 2) {
            // only steal if there are enough workers
            size_t idx = 0;
            do {
              idx = m_rengine() % m_number_of_workers;
            } while(this != &m_workers[idx]); // Dont steel on ourself
            fun = m_workers[idx].m_jobs.take_head();
          }
        };
        while(alive) {
          if (m_jobs.empty()) {
            m_data.m_empty.notify_all();
            do_steal();
          } else {
            fun = m_jobs.take_head();
          }
          if (fun) fun->exec();
          delete fun;
        }
      });
    }

    std::random_device m_rdevice;
    std::default_random_engine m_rengine;
    std::thread    m_thread;
    scheduler_data m_data;
    queue_t m_jobs;

    volatile bool alive;

    template<typename T, typename... Xs>
    auto enqueue(work_item<T, Xs...>* ptr) {
      auto future = ptr->get_future();
      m_jobs.append(ptr);
      return future;
    }

    template<typename T, typename... Xs>
    auto enqueue(T&& t, Xs&&... xs) {
      return enqueue(new work_item<T, Xs...>(std::move(t),
                                             std::forward<Xs>(xs)...));
    }

    template<typename T, typename... Xs>
    auto enqueue(T& t, Xs&... xs) {
      return enqueue(new work_item<T, Xs...>(t, xs...));
    }
  };

  friend worker;

  work_stealing() = default;

 public:
  template<typename T, typename... Xs>
  auto run(T&& t, Xs&&... xs) {
    size_t enqueu_to = ++m_next_enque % m_number_of_workers;
    return m_workers[enqueu_to].enqueue(std::move(t), std::forward<Xs>(xs)...);
  }

  template<typename T, typename... Xs>
  auto run(T& t, Xs&... xs) {
    size_t enqueu_to = ++m_next_enque % m_number_of_workers;
    return m_workers[enqueu_to].enqueue(t, xs...);
  }

  static work_stealing& get_instance();
  void shutdown();

 private:
  static std::mutex s_mtx;
  static work_stealing* instance;

  std::atomic<size_t> m_next_enque;
  static worker m_workers[m_number_of_workers];
};

#endif // WORK_STEALING_HPP
