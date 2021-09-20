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

#include "taski/detail/spinlock.hpp"

#include <catch2/catch2.hpp>

#include <future>

using namespace taski::detail;

#define ITERATIONS 314263153U

TEST_CASE("critical section", "[spinlock]") {
  std::atomic_bool flag = false;
  std::size_t critical = 0;

  auto fun = [&flag, &critical] {
    for (std::size_t i = 0; i < ITERATIONS; ++i) {
      spinlock s{flag};
      critical++;
    }
  };

  auto f1 = std::async(std::launch::async, fun);
  auto f2 = std::async(std::launch::async, fun);
  f1.wait();
  f2.wait();
  REQUIRE(critical == ITERATIONS * 2);
}