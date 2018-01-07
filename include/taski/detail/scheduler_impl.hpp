/*******************************************************************************
 *                            _            _    _                              *
 *                           | |          | |  (_)                             *
 *                           | |_ __ _ ___| | ___                              *
 *                           | __/ _` / __| |/ / |                             *
 *                           | || (_| \__ \   <| |                             *
 *                            \__\__,_|___/_|\_\_|                             *
 *                                                                             *
 * Copyright (C) 2015 - 2018                                                   *
 * Marian Triebe                                                               *
 *                                                                             *
 * Distributed under the terms and conditions of the BSD 3-Clause License      *
 * See accompanying file LICENSE.                                              *
 *                                                                             *
 * If you did not receive a copy of the license file, see                      *
 * http://opensource.org/licenses/BSD-3-Clause                                 *
 *******************************************************************************/

#ifndef SCHEDULER_IMPL_HPP
#define SCHEDULER_IMPL_HPP

#include <cstddef>
#include <utility>

namespace taski {

namespace detail {

/// Scheduler implementation.
/// @tparam Policy scheduler policy
template <class Policy>
class scheduler_impl : private Policy {
  using Policy::init;
  using Policy::internal_enqueue;
  using Policy::internal_dequeue;
  using Policy::shutdown;
public:
  constexpr scheduler_impl() {
    init();
  }

  ~scheduler_impl() {
    shutdown();
  }

  /// Enqueue a task to the scheduler.
  /// @param t Function to enqueue
  /// @param ts Arguments to be invoked with function
  /// @returns a std::future specialized for the return type of passed function
  ///          or std::future<void> if function is void.
  template <class T, class... Ts>
  auto enqueue(T&& t, Ts&&... ts) {
    static_assert(std::is_invocable_v<T, Ts...>);
    return internal_enqueue(std::forward<T>(t), std::forward<Ts>(ts)...);
  }
};

} // namespace detail

} // namespace taski

#endif // SCHEDULER_IMPL_HPP
