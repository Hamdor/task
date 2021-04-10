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

#include "taski/all.hpp"

using namespace taski;

struct copy_move {};

struct copy_only {
  copy_only() = default;
  copy_only(const copy_only&) = default;
  copy_only& operator=(const copy_only&) = default;

  copy_only(copy_only&&) = delete;
  copy_only& operator=(copy_only&&) = delete;
};

struct move_only {
  move_only() = default;
  move_only(const move_only&) = delete;
  move_only& operator=(const move_only&) = delete;

  move_only(move_only&&) = default;
  move_only& operator=(move_only&&) = default;
};

struct no_move_no_delete {
  no_move_no_delete() = default;
  no_move_no_delete(const no_move_no_delete&) = delete;
  no_move_no_delete& operator=(const no_move_no_delete&) = delete;

  no_move_no_delete(no_move_no_delete&&) = delete;
  no_move_no_delete& operator=(no_move_no_delete&&) = delete;
};

/// Compiles if move only types works as expected.
template <template <size_t> class Policy>
void test_move() {
  scheduler<Policy, 4> s;
  s.enqueue([](move_only c) { return c; }, move_only{});
  move_only m;
  s.enqueue([x = std::move(m)]() mutable { return std::move(x); });
}

/// Compiles if copy only types works as expected.
template <template <size_t> class Policy>
void test_copy() {
  scheduler<Policy, 2> s;
  copy_only c;
  s.enqueue([c] { return c; });
  s.enqueue([](copy_only c) { return c; }, c);
}

template <template <size_t> class Policy>
void test_reference() {
  scheduler<Policy, 4> s;
  move_only m{};
  copy_only c{};
  s.enqueue([](move_only&, const copy_only&) { }, m, c);
  s.enqueue([](copy_only&, const move_only&) { }, c, m);
  s.enqueue([](move_only&, copy_only&) { }, m, c);
  no_move_no_delete obj;
  s.enqueue([&obj](const no_move_no_delete&, no_move_no_delete&) { }, obj, obj);
}

template <template <size_t> class Policy>
void test_mixed() {
  scheduler<Policy, 4> s;
  copy_only c{};
  s.enqueue([](copy_only, move_only) { }, c, move_only{});
  s.enqueue([](move_only, copy_only) { }, move_only{}, c);
}

int main() {
  test_copy<stealing>();
  test_move<stealing>();
  test_reference<stealing>();
  test_mixed<stealing>();
  test_copy<sharing>();
  test_move<sharing>();
  test_reference<sharing>();
  test_mixed<sharing>();
  return 0;
}