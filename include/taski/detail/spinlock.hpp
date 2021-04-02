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

#include <atomic>
#include <thread>

/// Spinlock implementation.
struct spinlock {
  spinlock(std::atomic_flag& lock) : m_lock{lock} {
    while (lock.test_and_set(std::memory_order_acquire))
      std::this_thread::yield();
  }

  ~spinlock() {
    m_lock.clear(std::memory_order_release);
  }

  spinlock(const spinlock&) = delete;
  spinlock(spinlock&&) = delete;

  spinlock& operator=(const spinlock&) = delete;
  spinlock& operator=(spinlock&&) = delete;
private:
  std::atomic_flag& m_lock;   /// Reference to CAS flag to spin.
};