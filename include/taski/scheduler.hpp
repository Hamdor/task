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

#include "taski/detail/dynamic.hpp"
#include "taski/detail/scheduler_impl.hpp"

namespace taski {

/// Use 'taski::dynamic' to set the amount of workers to the
/// systems number of hardware threads. This is determined by
/// 'std::thread::hardware_concurrency()'.
static constexpr int dynamic = detail::dynamic;

/// @tparam Policy The policy for the scheduler, e.g. 'stealing'.
/// @tparam Workers The number of threads to be used.
template <template <int> class Policy, int Workers>
class scheduler : private detail::scheduler_impl<Policy<Workers>> {
  using impl = detail::scheduler_impl<Policy<Workers>>;
public:
  using impl::enqueue;

  scheduler() = default;
  scheduler(scheduler&&) = delete;
  scheduler(const scheduler&) = delete;
};

} // namespace taski
