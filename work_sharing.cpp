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
  { // Life scope of unique_lock
    std::unique_lock<std::mutex> lock(m_lock);
    while(!m_jobs.empty()) {
      m_cond_empty.wait(lock);
    }
  }
  for (worker& w : m_workers) {
    w.alive = false;
  }
  // spawn some dummy jobs... to get workers out of their wait condition
  // TODO: Cleaner way...
  auto dummy = []() {
    // nop
  };
  for (size_t i = 0; i < 8; ++i) {
    run(dummy);
  }
  for (worker& w : m_workers) {
    w.m_thread.join();
  }
  delete instance;
  instance = nullptr;
}
