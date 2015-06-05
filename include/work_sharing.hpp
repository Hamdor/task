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

#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <future>
#include <condition_variable>

#include "work_item.hpp"

class work_sharing {
  struct worker {
    worker() {
      alive = true;
      m_thread = std::thread([&]() {
        while(work_sharing::instance == nullptr) {
          // wait till instance is initialized...
        }
        auto& ws = *work_sharing::instance;
        while(alive) {
          std::unique_ptr<storeable> fun = nullptr;
          { // Lifetime scope of unique_lock
            std::unique_lock<std::mutex> lock(ws.m_lock);
            while (ws.m_jobs.empty()) {
              if (!alive) {
                return;
              }
              ws.m_cond_new.wait(lock);
            }
            fun = std::move(ws.m_jobs.front());
            ws.m_jobs.pop();
            if (ws.m_jobs.empty()) {
              ws.m_cond_empty.notify_all();
            }
          }
          if (fun) {
            fun->exec();
          }
        }
      });
    }
    std::thread m_thread;
    volatile bool alive;
  };

  friend worker;

  work_sharing() = default;

 public:
  template<typename T, typename... Xs>
  typename std::enable_if<
    std::is_void<typename std::result_of<T(Xs...)>::type>{}
  >::type
  run(T&& t, Xs&&... xs) {
    std::unique_lock<std::mutex> lock(m_lock);
    m_jobs.emplace(new work_item<T, Xs...>(std::move(t),
                                           std::forward<Xs>(xs)...));
    m_cond_new.notify_all();
  }

  template<typename T, typename... Xs>
  typename std::enable_if<
    !std::is_void<typename std::result_of<T(Xs...)>::type>{},
    typename std::future<typename std::result_of<T(Xs...)>::type>
  >::type
  run(T&& t, Xs&&... xs) {
    auto job = new work_item<T, Xs...>(std::move(t),
                                       std::forward<Xs>(xs)...);
    std::unique_lock<std::mutex> lock(m_lock);
    m_jobs.emplace(job);
    m_cond_new.notify_all();
    return job->get_future();
  }

  template<typename T, typename... Xs>
  typename std::enable_if<
    std::is_void<typename std::result_of<T(Xs...)>::type>{}
  >::type
  run(T& t, Xs&... xs) {
    std::unique_lock<std::mutex> lock(m_lock);
    m_jobs.emplace(new work_item<T, Xs...>(t, xs...));
    m_cond_new.notify_all();
  }

  template<typename T, typename... Xs>
  typename std::enable_if<
    !std::is_void<typename std::result_of<T(Xs...)>::type>{},
    typename std::future<typename std::result_of<T(Xs...)>::type>
  >::type
  run(T& t, Xs&... xs) {
    auto job = new work_item<T, Xs...>(t, xs...);
    std::unique_lock<std::mutex> lock(m_lock);
    m_jobs.emplace(job);
    m_cond_new.notify_all();
    return job->get_future();
  }

  static work_sharing& get_instance();
  void shutdown();

 private:
  static std::mutex s_mtx;
  static work_sharing* instance;

  std::mutex m_lock;
  std::condition_variable m_cond_empty;
  std::condition_variable m_cond_new;
  std::queue<std::unique_ptr<storeable>> m_jobs;
  worker m_workers[8]; // TODO: use hw concurrency...
};

#endif // WORK_SHARING_HPP
