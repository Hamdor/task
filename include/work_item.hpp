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

#ifndef WORK_ITEM_HPP
#define WORK_ITEM_HPP

#include <tuple>
#include <future>
#include <type_traits>

#include "storeable.hpp"
#include "tuple_unpacking.hpp"

template<typename T, typename... Xs>
struct work_item : public storeable {
  work_item(T&& t, Xs&&... xs)
    : m_fun(std::move(t))
    , m_args(std::make_tuple(std::forward<Xs>(xs)...)) {
    // nop
  }

  work_item(T& t, Xs&... xs)
    : m_fun(t)
    , m_args(std::make_tuple(xs...)) {
    // nop
  }

  using ret_type_t = typename std::result_of<T(Xs...)>::type;
  std::promise<ret_type_t> m_ret;

  T m_fun;
  std::tuple<Xs...> m_args;

  std::future<ret_type_t> get_future() {
    return m_ret.get_future();
  }

  virtual void exec() {
    call_unpack(typename unpack_tuple<sizeof...(Xs)>::type());
  }

  template<typename F = ret_type_t, int... S>
  typename std::enable_if<!std::is_void<F>{}, F>::type
  call_unpack(unpacked<S...>) {
    auto resu = m_fun(std::get<S>(m_args)...);
    m_ret.set_value(resu);
    return resu;
  }

  template<typename F = ret_type_t, int... S>
  typename std::enable_if<std::is_void<F>{}>::type
  call_unpack(unpacked<S...>) {
    m_fun(std::get<S>(m_args)...);
  }
};

#endif // WORK_ITEM_HPP
