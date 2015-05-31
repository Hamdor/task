#include "work_sharing.hpp"

std::mutex work_sharing::s_mtx;
work_sharing* work_sharing::instance = nullptr;

work_sharing& work_sharing::get_instance() {
  if (instance == nullptr) {
    std::unique_lock<std::mutex> lock(s_mtx);
    if (instance == nullptr) {
      instance = new work_sharing;
    }
  }
  return *instance;
}

void work_sharing::shutdown() {
  while(!m_jobs.empty()) {
    // wait till all jobs are consumed
    // TODO: Cond var here
  }
  for (worker& w : m_workers) {
    w.alive = false;
  }
  for (worker& w : m_workers) {
    w.m_thread.join();
  }
}
