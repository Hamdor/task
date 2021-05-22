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

#include <array>
#include <limits>
#include <cstdint>

namespace taski::detail
{

/**
 * @tparam X, Y, Z, W, V Shall be non zero
 * @tparam D any value including zero
 *
 * xorwow implementation from:
 * https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf
 */
template<uint32_t X, uint32_t Y, uint32_t Z, uint32_t W, uint32_t V, uint32_t D>
class xorwow_engine {
public:
  using result_type = uint32_t;

  static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
  static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

  result_type operator()() {
    uint32_t t = (state[0] ^ (state[0] >> 2));
    state[0] = state[1];
    state[1] = state[2];
    state[2] = state[3];
    state[3] = state[4];
    state[4] = (state[4] ^ (state[4] << 4)) ^ (t ^ (t << 1));
    state[5] += 362437;
    return state[5] + state[4];
  }
private:
  using state_t = std::array<result_type, 6>;
  state_t state = {X, Y, Z, W, V, D};
};

/// Initialization parameters from original paper,
/// https://www.jstatsoft.org/article/view/v008i14/xorshift.pdf
using xorwow = xorwow_engine<
  123456789, 362436069, 521288629, 88675123, 5783321, 6615241>;

}  // namespace taski::detail