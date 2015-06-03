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

  using ret_type = typename std::result_of<T(Xs...)>::type;
  using ret_type_t = std::conditional_t<
    std::is_same<ret_type, void>::value, int, ret_type>;
  ret_type_t m_ret;

  T m_fun;
  std::tuple<Xs...> m_args;

  virtual void exec() {
    call_unpack(typename unpack_tuple<sizeof...(Xs)>::type());
  }

  template<typename F = ret_type, int... S>
  typename std::enable_if<!std::is_void<F>{}, F>::type
  call_unpack(unpacked<S...>) {
    m_ret = m_fun(std::get<S>(m_args)...);
    return m_ret;
  }

  template<typename F = ret_type, int... S>
  typename std::enable_if<std::is_void<F>{}>::type
  call_unpack(unpacked<S...>) {
    m_fun(std::get<S>(m_args)...);
  }
};

#endif // WORK_ITEM_HPP
