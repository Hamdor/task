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

#include <cstddef>
#include <utility>

namespace taski::detail {

/// Scheduler implementation.
/// @tparam Policy scheduler policy
template <class Policy>
class scheduler_impl : private Policy {
  using Policy::internal_enqueue;
public:
  scheduler_impl() = default;

  ~scheduler_impl() override = default;

  /// Enqueue a task to the scheduler.
  /// @param fun Function to enqueue
  /// @param args Arguments to be invoked with function
  /// @returns a std::future specialized for the return type of passed function
  ///          or std::future<void> if function is void.
  template <class Fun, class... Args>
  auto enqueue(Fun&& fun, Args&&... args) {
    return internal_enqueue(std::forward<Fun>(fun), std::forward<Args>(args)...);
  }
};

} // namespace taski::detail
