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

#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <future>
#include <atomic>
#include <condition_variable>

#include "work_item.hpp"

class work_stealing {
  struct worker {
    worker() {
      alive = true;
      m_thread = std::thread([&]() {
        while(work_stealing::instance == nullptr) {
          // wait till instance is initialized...
        }
        auto& ws = *work_stealing::instance;
        while(alive) {
          std::unique_ptr<storeable> fun = nullptr;
          { // Lifetime scope of unique_lock
            std::unique_lock<std::mutex> lock(m_lock);
            if (m_jobs.empty()) {
              m_empty.notify_all();
              lock.unlock();
              size_t idx = 0;
              do {
                // TODO: Steal at random worker
                idx = (idx + 1) % m_number_of_workers;
              } while(this != &m_workers[idx]); // Dont steel on ourself
              std::unique_lock<std::mutex> steal_lock(m_workers[idx].m_lock);
              if (m_workers[idx].m_jobs.empty()) {
                steal_lock.unlock();
                lock.lock();
                m_new.wait(lock);
                continue;
              }
              fun = std::move(m_workers[idx].m_jobs.front());
              m_workers[idx].m_jobs.pop();
            } else {
              fun = std::move(m_jobs.front());
              m_jobs.pop();
            }
          }
          if (fun) {
            fun->exec();
          }
        }
      });
    }
    std::thread m_thread;
    std::mutex  m_lock;
    std::condition_variable m_empty;
    std::condition_variable m_new;
    std::queue<std::unique_ptr<storeable>> m_jobs;
    volatile bool alive;

    template<typename T, typename... Xs>
    auto enqueue(work_item<T, Xs...>* ptr) {
      std::unique_lock<std::mutex> lock(m_lock);
      m_jobs.emplace(ptr);
      m_new.notify_all();
      return ptr->get_future();
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

  std::mutex m_lock;
  std::condition_variable m_cond_empty;
  std::condition_variable m_cond_new;
  std::atomic<size_t> m_next_enque;
  constexpr static size_t m_number_of_workers = 8; // TODO: use hw concurrency...
  static worker m_workers[m_number_of_workers];
};

#endif // WORK_STEALING_HPP
