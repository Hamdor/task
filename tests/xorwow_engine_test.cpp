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

#include "taski/detail/xorwow_engine.hpp"

#include <catch2/catch2.hpp>

using namespace taski::detail;

TEST_CASE("roll", "[xorwow_engine]") {
  xorwow generator;
  // Expected first values for default seed
  REQUIRE(generator.operator()() == 246875399);
  REQUIRE(generator.operator()() == 3690007200);
  REQUIRE(generator.operator()() == 1264581005);
  REQUIRE(generator.operator()() == 3906711041);
}

TEST_CASE("dist", "[xorwow_engine]") {
  xorwow generator;
  // Ensure compatiblity with 'uniform_int_distribution'
  std::uniform_int_distribution<size_t> dist(0, 100);
  REQUIRE(dist(generator) == 5);
}