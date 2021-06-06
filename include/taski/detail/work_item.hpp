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
  using result_t = std::invoke_result_t<Fun, Args...>;
public:
  /// Creates a work item composed of a function and its arguments.
  /// @param fun Function to be invoked
  /// @param args Arguments to be applied to function
  template <class T, class... Ts>
  work_item(T&& fun, Ts&&... args)
    : fun_{std::forward<T>(fun)}
    , args_{std::forward_as_tuple(std::forward<Ts>(args)...)} {
    // nop
  }

  /// @returns a future which is set when the function has been called.
  std::future<result_t> future() { return ret_.get_future(); }

  /// Invoke the stored function with stored arguments.
  void operator()() override {
    if constexpr (std::is_same_v<result_t, void>) {
      std::apply(fun_, std::move(args_));
      ret_.set_value();
    } else {
      auto res = std::apply(fun_, std::move(args_));
      if constexpr (std::is_move_constructible_v<result_t>) {
        ret_.set_value(std::move(res));
      } else {
        ret_.set_value(res);
      }
    }
  }

private:
  Fun fun_;                   /// Function to be invoked
  std::tuple<Args...> args_;  /// Arguments stored as tuple
  std::promise<result_t> ret_;  /// Response promise
};

} // namespace taski::detail
