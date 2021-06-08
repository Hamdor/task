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

#include "taski/detail/index_table.hpp"

#include <catch2/catch2.hpp>

using namespace taski::detail;

TEST_CASE("Empty table is empty", "[index_table]") {
  auto table = generate_index_table<0>(0);
  REQUIRE(table.empty());
}

TEST_CASE("Index table skips correct index (begin)", "[index_table]") {
  auto table = generate_index_table<3>(0);
  REQUIRE(table[0] == 1);
  REQUIRE(table[1] == 2);
  REQUIRE(table[2] == 3);
}

TEST_CASE("Index table skips correct index (median)", "[index_table]") {
  auto table = generate_index_table<3>(1);
  REQUIRE(table[0] == 0);
  REQUIRE(table[1] == 2);
  REQUIRE(table[2] == 3);
}

TEST_CASE("Index table skips correct index (end)", "[index_table]") {
  auto table = generate_index_table<3>(2);
  REQUIRE(table[0] == 0);
  REQUIRE(table[1] == 1);
  REQUIRE(table[2] == 3);
}
