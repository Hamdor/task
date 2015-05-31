

#ifndef WORK_SHARING_HPP
#define WORK_SHARING_HPP

#include <mutex>
#include <queue>
#include <thread>
#include <memory>

#include <iostream>

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
          while(ws.m_jobs.empty()) {
            // TODO: Cond var here...
            //if (!alive) { return; }
          }
          { // Lifetime scope of unique_lock
            std::unique_lock<std::mutex> lock(ws.m_lock);
            if (ws.m_jobs.empty()) { continue; }
            fun = std::move(ws.m_jobs.front());
            ws.m_jobs.pop();
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
  void run(T&& t, Xs&&... xs) {
    m_jobs.emplace(new work_item<T, Xs...>(std::move(t),
                                           std::forward<Xs>(xs)...));
  }

  static work_sharing& get_instance();
  void shutdown();

 private:
  static std::mutex s_mtx;
  static work_sharing* instance;

  std::mutex m_lock;
  std::queue<std::unique_ptr<storeable>> m_jobs;
  worker m_workers[4]; // TODO: use hw concurrency...
};

#endif // WORK_SHARING_HPP
