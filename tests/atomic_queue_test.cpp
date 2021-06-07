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

#include "taski/detail/atomic_queue.hpp"

#include <catch2/catch2.hpp>

using namespace taski::detail;

TEST_CASE("Empty queue is empty", "[atomic_queue]") {
  atomic_queue<int> queue;
  REQUIRE(queue.empty());
  REQUIRE(queue.take_head() == nullptr);
}

TEST_CASE("Filled queue is not empty", "[atomic_queue]") {
  atomic_queue<int> queue;
  queue.append(std::make_unique<int>(2));
  REQUIRE_FALSE(queue.empty());
  auto elem = queue.take_head();
  REQUIRE(*elem == 2);
  REQUIRE(queue.empty());
}

TEST_CASE("Non empty destruction", "[atomic_queue]") {
  atomic_queue<int> queue;
  queue.append(std::make_unique<int>(2));
  queue.append(std::make_unique<int>(3));
  queue.append(std::make_unique<int>(4));
}
