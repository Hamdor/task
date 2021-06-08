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

#include "taski/detail/work_item.hpp"

#include "types.hpp"

#include <catch2/catch2.hpp>

using namespace taski::detail;

TEST_CASE("void(void) function", "[work_item]") {
  auto fun = [] {};
  work_item<decltype(fun)> w{fun};
  auto future = w.future();
  w();
  future.get();
}

TEST_CASE("int(void) function", "[work_item]") {
  auto fun = [] { return 15; };
  work_item<decltype(fun)> w{fun};
  auto future = w.future();
  w();
  REQUIRE(future.get() == 15);
}

TEST_CASE("copy_only(void) function", "[work_item]") {
  auto fun = [] { return copy_only{}; };
  work_item<decltype(fun)> w{fun};
  w();
}

TEST_CASE("move_only(void) function", "[work_item]") {
  auto fun = [] { return move_only{}; };
  work_item<decltype(fun)> w{fun};
  w();
}

TEST_CASE("void(int) function", "[work_item]") {
  auto fun = [](int) { };
  work_item<decltype(fun), int> w{fun, int{2}};
  w();
}

TEST_CASE("void(copy_only) function", "[work_item]") {
  auto fun = [](copy_only) { };
  copy_only obj;
  work_item<decltype(fun), const copy_only&> w{fun, obj};
  w();
}

TEST_CASE("void(move_only) function", "[work_item]") {
  auto fun = [](move_only) { };
  work_item<decltype(fun), move_only&&> w{fun, move_only{}};
  w();
}

TEST_CASE("copy only function", "[work_item]") {
  copy_only obj;
  auto fun = [obj] { };
  work_item<decltype(fun)> w{fun};
  w();
}

TEST_CASE("move only function", "[work_item]") {
  move_only obj;
  auto fun = [obj=std::move(obj)] { };
  work_item<decltype(fun)> w{std::move(fun)};
  w();
}
