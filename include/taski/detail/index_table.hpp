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

#include <vector>
#include <numeric>

namespace taski::detail {

/// Generates an array of given Size.
/// @param size The size of the array.
/// @param skip_idx The index value to be skipped, i.e.
///        The values of the array are as follows:
///          - for range [0, skip_idx) we have the values [0, skip_idx)
///          - for range [skip_idx, Size) we have the values [skip_idx+1, Size)
///        @pred skip_idx is not bigger than Size.
static std::vector<std::size_t> generate_index_table(std::size_t size, std::size_t skip_idx) {
  std::vector<std::size_t> table(size, 0u);
  auto until = table.begin() + skip_idx;
  std::iota(table.begin(), until, 0);
  std::iota(until, table.end(), skip_idx + 1);
  return table;
}

} // namespace taski::detail
