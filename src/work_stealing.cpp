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

#include "work_stealing.hpp"

std::mutex work_stealing::s_mtx;
work_stealing* work_stealing::instance = nullptr;
work_stealing::worker work_stealing::m_workers[];

work_stealing& work_stealing::get_instance() {
  if (instance == nullptr) {
    std::unique_lock<std::mutex> lock(s_mtx);
    if (instance == nullptr) {
      instance = new work_stealing;
    }
  }
  return *instance;
}

void work_stealing::shutdown() {
  for (worker& w : m_workers) {
    std::unique_lock<std::mutex> lock(w.m_data.m_lock);
    while(!w.m_jobs.empty()) {
      w.m_data.m_empty.wait(lock);
    }
  }
  for (worker& w : m_workers) {
    w.alive = false;
  }
  // Spawn some dummy jobs to get workers out of thei blocking state
  for (size_t i = 0; i < CONCURRENCY_LEVEL*2; ++i) {
    run([]() {});
  }
  for (worker& w : m_workers) {
    w.m_thread.join();
  }
  delete instance;
  instance = nullptr;
}
