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

#include "taski/scheduler.hpp"

#include "taski/policy/sharing.hpp"
#include "taski/policy/stealing.hpp"

#include <catch2/catch2.hpp>

#include <chrono>

using namespace taski;

using namespace std::chrono_literals;

template <std::size_t workers>
struct fake_policy {
  virtual ~fake_policy() {
    REQUIRE(internal_enqueue_called);
  }

  template <class T, class... Ts>
  decltype(auto) internal_enqueue(T&&, Ts&&...) {
    internal_enqueue_called = true;
  }

  bool internal_enqueue_called = false;
};

TEST_CASE("Policy function called", "[scheduler]") {
  scheduler<fake_policy, 1> scheduler;
  scheduler.enqueue([]{});
}

TEST_CASE("work sharing policy", "[scheduler]") {
  scheduler<sharing, 2> scheduler;
  auto fun = []{ std::this_thread::sleep_for(50ms); };
  for (int i = 0; i < 10; ++i) {
    scheduler.enqueue(fun);
  }
}

TEST_CASE("work stealing policy", "[scheduler]") {
  scheduler<stealing, 2> scheduler;
  auto fun = []{ std::this_thread::sleep_for(50ms); };
  for (int i = 0; i < 10; ++i) {
    scheduler.enqueue(fun);
  }
}

TEST_CASE("work stealing dynamic workers", "[scheduler]") {
  scheduler<stealing, dynamic> scheduler;
  auto fun = []{ std::this_thread::sleep_for(50ms); };
  for (int i = 0; i < 10; ++i) {
    scheduler.enqueue(fun);
  }
}

