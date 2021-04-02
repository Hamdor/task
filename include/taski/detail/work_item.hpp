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

#include <tuple>
#include <future>
#include <functional>

#include "taski/detail/storable.hpp"

namespace taski::detail {

/// Stores a function and its arguments as a whole.
template <class Fun, class... Args>
class work_item : public storable {
  using result = std::invoke_result_t<Fun, Args...>;
public:
  /// Creates a work item composed of a function and its arguments.
  /// @param fun Function to be invoked
  /// @param args Arguments to be applied to function
  work_item(Fun&& fun, Args&&... args)
    : fun_{std::forward<Fun>(fun)}
    , args_{std::forward_as_tuple(std::forward<Args>(args)...)} {
    // nop
  }

  /// @returns a future which is set when the function has been called.
  auto future() { return ret_.get_future(); }

  /// Invoke the stored function with stored arguments.
  void operator()() override {
    if constexpr (std::is_same_v<result, void>) {
      std::apply(fun_, args_);
      ret_.set_value();
    } else
      ret_.set_value(std::apply(fun_, args_));
  }

private:
  Fun fun_;                   /// Function to be invoked
  std::tuple<Args...> args_;  /// Arguments stored as tuple
  std::promise<result> ret_;  /// Response promise
};

} // namespace taski::detail
